/*
 * Copyright (C) 2013, The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>
#include <cutils/properties.h>
#include <sys/stat.h>
#include <termios.h>

#include <system/audio.h>
#include <platform.h>
#include <audio_hw.h>
#include <sound/es310.h>

#define LOG_NDEBUG 0
#define LOG_TAG "audio_amplifier"
#include <cutils/log.h>

#include <hardware/audio_amplifier.h>

#define UNUSED __attribute__ ((unused))

typedef struct amp_device {
	amplifier_device_t amp_dev;
	audio_mode_t mode;

	int mES310Fd;
	pthread_mutex_t mES310Mutex;

	enum ES310_PathID dwPath;
	unsigned int dwPreset;

	snd_device_t in_snd_device;
	snd_device_t out_snd_device;
} amp_device_t;

struct voiceproc_img
{
	unsigned char *buf;
	unsigned img_size;
};

static amp_device_t *amp_dev = NULL;

static const char *es310_getNameByPresetID(int presetID)
{
	switch (presetID) {
	case ES310_PRESET_HANDSET_INCALL_NB:
		return "ES310_PRESET_HANDSET_INCALL_NB";
	case ES310_PRESET_HEADSET_INCALL_NB:
		return "ES310_PRESET_HEADSET_INCALL_NB";
	case ES310_PRESET_HANDSET_INCALL_NB_1MIC:
		return "ES310_PRESET_HANDSET_INCALL_NB_1MIC";
	case ES310_PRESET_HANDSFREE_INCALL_NB:
		return "ES310_PRESET_HANDSFREE_INCALL_NB";
	case ES310_PRESET_HANDSET_INCALL_WB:
		return "ES310_PRESET_HANDSET_INCALL_WB";
	case ES310_PRESET_HEADSET_INCALL_WB:
		return "ES310_PRESET_HEADSET_INCALL_WB";
	case ES310_PRESET_AUDIOPATH_DISABLE:
		return "ES310_PRESET_AUDIOPATH_DISABLE";
	case ES310_PRESET_HANDSFREE_INCALL_WB:
		return "ES310_PRESET_HANDSFREE_INCALL_WB";
	case ES310_PRESET_HANDSET_VOIP_WB:
		return "ES310_PRESET_HANDSET_VOIP_WB";
	case ES310_PRESET_HEADSET_VOIP_WB:
		return "ES310_PRESET_HEADSET_VOIP_WB";
	case ES310_PRESET_HANDSFREE_REC_WB:
		return "ES310_PRESET_HANDSFREE_REC_WB";
	case ES310_PRESET_HANDSFREE_VOIP_WB:
		return "ES310_PRESET_HANDSFREE_VOIP_WB";
	case ES310_PRESET_VOICE_RECOGNIZTION_WB:
		return "ES310_PRESET_VOICE_RECOGNIZTION_WB";
	case ES310_PRESET_HANDSET_INCALL_VOIP_WB_1MIC:
		return "ES310_PRESET_HANDSET_INCALL_VOIP_WB_1MIC";
	case ES310_PRESET_ANALOG_BYPASS:
		return "ES310_PRESET_ANALOG_BYPASS";
	case ES310_PRESET_HEADSET_MIC_ANALOG_BYPASS:
		return "ES310_PRESET_HEADSET_MIC_ANALOG_BYPASS";
	default:
		return "Unknown";
	}
	return "Unknown";
}

static const char *es310_getNameByPathID(int pathID)
{
	switch (pathID) {
	case ES310_PATH_SUSPEND:
		return "ES310_PATH_SUSPEND";
	case ES310_PATH_HANDSET:
		return "ES310_PATH_HANDSET";
	case ES310_PATH_HEADSET:
		return "ES310_PATH_HEADSET";
	case ES310_PATH_HANDSFREE:
		return "ES310_PATH_HANDSFREE";
	case ES310_PATH_BACKMIC:
		return "ES310_PATH_BACKMIC";
	default:
		return "Unknown";
	}
	return "Unknown";
}

#define BUFSIZE_UART 1024
#define VOICEPROC_MAX_FW_SIZE	(32 * 4096)

static void setHigherBaudrate(int uart_fd, int baud)
{
	struct termios2 ti2;
       struct termios ti;
	/* Flush non-transmitted output data,
	 * non-read input data or both
	 */
	tcflush(uart_fd, TCIFLUSH);

	/* Set the UART flow control */

	ti.c_cflag |= 1;

	/* ti.c_cflag |= (CLOCAL | CREAD | CSTOPB); */
	/*	ti.c_cflag &= ~(CRTSCTS | PARENB); */

	/*
	 * Enable the receiver and set local mode + 2 STOP bits
	 */
	ti.c_cflag |= (CLOCAL | CREAD | CSTOPB);
	/* 8 data bits */
	ti.c_cflag &= ~CSIZE;
	ti.c_cflag |= CS8;
	/* diable HW flow control and parity check */
	ti.c_cflag &= ~(CRTSCTS | PARENB);

	/* choose raw input */
	ti.c_lflag &= ~(ICANON | ECHO);
	/* choose raw output */
	ti.c_oflag &= ~OPOST;
	/* ignore break condition, CR and parity error */
	ti.c_iflag |= (IGNBRK | IGNPAR);
	ti.c_iflag &= ~(IXON | IXOFF | IXANY);
	ti.c_cc[VMIN] = 0;
	ti.c_cc[VTIME] = 10;

	/*
	 * Set the parameters associated with the UART
	 * The change will occur immediately by using TCSANOW
	 */
	if (tcsetattr(uart_fd, TCSANOW, &ti) < 0) {
		printf("Can't set port settings\n");
		return;
	}

	tcflush(uart_fd, TCIFLUSH);

	/* Set the actual baud rate */
	ioctl(uart_fd, TCGETS2, &ti2);
	ti2.c_cflag &= ~CBAUD;
	ti2.c_cflag |= BOTHER;
	ti2.c_ospeed = baud;
	ti2.c_ispeed = baud;
	ioctl(uart_fd, TCSETS2, &ti2);
}

int sendDownloadCmd(int uart_fd)
{
	unsigned char respBuffer = 0xcc, tmp;
	int nretry = 10, rc;
	int BytesWritten, readCnt;

	tmp = 0x00;
	BytesWritten = write(uart_fd, &tmp, 1);
	if (BytesWritten == -1)
		ALOGE("error writing synccmd to comm port: %s\n", strerror(errno));
	else
		ALOGE("Uart_write BytesWritten = %i\n", BytesWritten);

	usleep(1000);
	readCnt = read(uart_fd, &respBuffer, 1);
	if (readCnt == -1)
		ALOGE("error reading bootcmd from comm port: %s\n", strerror(errno));
	else
		ALOGE("readCnt = %d, respBuffer = %.2x\n", readCnt, respBuffer);
	usleep(1000);

	tmp = 0x01;
	BytesWritten = write(uart_fd, &tmp, 1);
	if (BytesWritten == -1)
		ALOGE("error writing bootcmd to comm port: %s\n", strerror(errno));
	else
		ALOGE("Uart_write BytesWritten = %i\n", BytesWritten);

	usleep(1000);
	readCnt = read(uart_fd, &respBuffer, 1);
	if (readCnt == -1)
		ALOGE("error reading bootcmd from comm port: %s\n", strerror(errno));
	else
		ALOGE("readCnt = %d, respBuffer = %.2x\n", readCnt, respBuffer);

	if (respBuffer == 1)
		return 0;

	return -1;
}

int uartSendBinaryFile(int uart_fd, const char* img)
{
	int ret = -1;
	unsigned i = 0, write_size = 0;
	ALOGE("voiceproc_uart_sendImg %s\n", img);
	struct voiceproc_img fwimg;
	char char_tmp = 0;
	unsigned char local_vpimg_buf[VOICEPROC_MAX_FW_SIZE], *ptr = local_vpimg_buf;
	int rc = 0, fw_fd = -1;
	ssize_t nr;
	size_t remaining;
	struct stat fw_stat;

	fw_fd = open(img, O_RDONLY);
	if (fw_fd < 0) {
		ALOGE("Fail to open %s\n", img);
              rc = -1;
		goto ld_img_error;
	}

	rc = fstat(fw_fd, &fw_stat);
	if (rc < 0) {
		ALOGE("Cannot stat file %s: %s\n", img, strerror(errno));
              rc = -1;
		goto ld_img_error;
	}

	remaining = (int)fw_stat.st_size;

	ALOGV("Firmware %s size %d\n", img, remaining);

	if (remaining > sizeof(local_vpimg_buf)) {
		ALOGE("File %s size %d exceeds internal limit %d\n",
			 img, remaining, sizeof(local_vpimg_buf));
              rc = -1;
		goto ld_img_error;
	}

	while (remaining) {
		nr = read(fw_fd, ptr, remaining);
		if (nr < 0) {
			ALOGE("Error reading firmware: %s\n", strerror(errno));
                     rc=-1;
			goto ld_img_error;
		} else if (!nr) {
			if (remaining)
				ALOGV("EOF reading firmware %s while %d bytes remain\n",
					 img, remaining);
			break;
		}
		remaining -= nr;
		ptr += nr;
	}

	close (fw_fd);
	fw_fd = -1;

	fwimg.buf = local_vpimg_buf;
	fwimg.img_size = (int)(fw_stat.st_size - remaining);
	ALOGV("voiceproc_uart_sendImg firmware Total %d bytes\n", fwimg.img_size);
	i = 0;
	write_size = 0;

	while (i < fwimg.img_size) {
		ret = write(uart_fd, fwimg.buf+i,
			(fwimg.img_size - i) < BUFSIZE_UART ? (fwimg.img_size-i) : BUFSIZE_UART);
		if (ret == -1)
              {
			ALOGV("Error, voiceproc uart write: %s\n", strerror(errno));
                     rc = -1;
                     goto ld_img_error;
              }

		write_size += ret;
		i += BUFSIZE_UART;
	}
	if (write_size != fwimg.img_size)
       {
		ALOGE("Error, UART writeCnt %d != img_size %d\n", write_size, fwimg.img_size);
              rc = -1;
              goto ld_img_error;
       }
	else
		ALOGV("UART writeCnt is %d verus img_size is %d\n", write_size, fwimg.img_size);

ld_img_error:
	if (fw_fd >= 0)
		close(fw_fd);
	return rc;
}

#define UART_INIT_IMAGE "/system/etc/firmware/voiceproc_init.img"
#define UART_DEV_NAME "/dev/ttyHS2"
int uart_load_binary(int fd, char *firmware_path)
{
    int ret = 0;
    int uart_fd = 0;
    int i;
    int retry_count = 100;
    struct termios options;
    struct termios2 options2;

    while (retry_count) {
        ret = ioctl(fd, ES310_RESET_CMD);
        if (!ret)
            ALOGV("ES310: voiceproc_reset ES310_RESET_CMD OK\n");
        else
            ALOGE("ES310: voiceproc_reset ES310_RESET_CMD error %s\n", strerror(errno));

        /* init uart port */
        uart_fd = open(UART_DEV_NAME, O_RDWR | O_NOCTTY | O_NDELAY);
        if (uart_fd < 0) {
            ALOGE("fail to open uart port %s\n", UART_DEV_NAME);
            return -1;
        }
        fcntl(uart_fd, F_SETFL, 0);

        /* First stage download */
        setHigherBaudrate(uart_fd, 28800);

        /* reset voice processor */
        usleep(10000);

        ret = sendDownloadCmd(uart_fd);
        if (ret) {
            ALOGE("error sending 1st download command on 1st stage\n");
            retry_count--;
            close(uart_fd);
            continue;
        }

        uartSendBinaryFile(uart_fd, UART_INIT_IMAGE);
        ALOGV("Send init image done\n");

        /* Second stage download */
        if (tcgetattr(uart_fd, &options) < 0) {
            ALOGE("Can't get port settings\n");
            retry_count--;
            close(uart_fd);
            continue;
        } else if (tcsetattr(uart_fd, TCSADRAIN, &options) < 0) {
            ALOGE("Can't set port settings\n");
            retry_count--;
            close(uart_fd);
            continue;
        }
        if (ioctl(uart_fd, TCGETS2, &options2) < 0) {
            ALOGE("Can't get port settings 2\n");
            retry_count--;
            close(uart_fd);
            continue;
        } else {
            options2.c_cflag &= ~CBAUD;
            options2.c_cflag |= BOTHER;
            options2.c_ospeed = 3000000;
            options2.c_ispeed = 3000000;
            if (ioctl(uart_fd, TCSETS2, &options2) < 0) {
                ALOGE("Can't set port settings 2\n");
                retry_count--;
                close(uart_fd);
                continue;
            }
        }

        usleep(10000);
        ret = sendDownloadCmd(uart_fd);
        if (ret) {
            ALOGE("ES310: error sending download command, abort. \n");
            ALOGE("ES310: retry_count:%d", 100 - retry_count);
            retry_count--;
            close(uart_fd);
            continue;
        } else {
            ALOGV("ES310: init send command done");
            break;
        }
    }

    if (ret) {
        ALOGE("ES310: initial codec command error");
        ret = -1;
        goto ERROR;
    }

    usleep(1000);
    ret = uartSendBinaryFile(uart_fd, firmware_path);

ERROR:
    close(uart_fd);

    return ret;
}

int es310_init(amp_device_t *dev)
{
	int rc = -1;

	ALOGD("%s\n", __func__);

	// open
	int mES310Fd = open("/dev/audience_es310", O_RDWR | O_NONBLOCK, 0);
	if (mES310Fd < 0) {
		ALOGE("%s: unable to open es310 device!", __func__);
		return rc;
	}
	// reset
	rc = ioctl(mES310Fd, ES310_RESET_CMD);
	if (rc) {
		ALOGE("%s: ES310_RESET_CMD fail, rc=%d", __func__, rc);
		close(mES310Fd);
		mES310Fd = -1;
		return rc;
	}

	// sync
	int retry_count = 20;
	while(retry_count)
	{
			ALOGV("start loading the voiceproc.img file, retry:%d +", 20 - retry_count);
			ALOGV("set codec reset command");
			rc = uart_load_binary(mES310Fd, "/etc/firmware/voiceproc.img");
			if (rc != 0)
			{
					ALOGE("uart_load_binary fail, rc:%d", rc);
					retry_count--;
					continue;
			}
			ALOGV("start loading the voiceproc.img file -");
			usleep(11000);
			ALOGV("ES310 SYNC CMD +");
			rc = ioctl(mES310Fd, ES310_SYNC_CMD, NULL);
			ALOGV("ES310 SYNC CMD, rc:%d-", rc);
			if (rc != 0)
			{
					ALOGE("ES310 SYNC CMD fail, rc:%d", rc);
					retry_count--;
					continue;
			}

			if (rc == 0)
					break;
	}
	dev->mES310Fd = mES310Fd;

	return rc;
}

static int es310_wakeup(amp_device_t *dev)
{
	int rc = -1;

	ALOGD("%s\n", __func__);

	if (dev->mES310Fd < 0) {
		ALOGE("%s: codec not initialized.\n", __func__);
		return rc;
	}

	int retry = 4;
	do {
		ALOGV("%s: ioctl ES310_SET_CONFIG retry:%d", __func__,
		      4 - retry);
		rc = ioctl(dev->mES310Fd, ES310_WAKEUP_CMD, NULL);
		if (!rc)
			break;
		else
			ALOGE("%s: ES310_SET_CONFIG fail rc=%d", __func__, rc);
	} while (--retry);

	return rc;
}

static int es310_do_route(amp_device_t *dev)
{
	int rc = -1;
	char cVNRMode[255] = "2";
	int VNRMode = 2;
	enum ES310_PathID dwNewPath = ES310_PATH_SUSPEND;
	unsigned int dwNewPreset = -1;

	audio_mode_t mMode = dev->mode;
	snd_device_t in_snd_device = dev->in_snd_device;
	enum ES310_PathID dwOldPath = dev->dwPath;
	unsigned int dwOldPreset = dev->dwPreset;

	if (dev->mES310Fd < 0) {
		ALOGE("%s: codec not initialized.\n", __func__);
		return rc;
	}

	pthread_mutex_lock(&dev->mES310Mutex);

	// original usecase: suspend if we don't use a mic
	// if we don't use any input device codec config doesn't matter.
	// and since we'll get NONE as device even if there are active devices
	// we just don't do anything in this case so we're not going to select wrong routes
	if ((in_snd_device == SND_DEVICE_NONE)) {
		ALOGD("%s: Normal mode, RX no routing\n", __func__);
		goto unlock;

		// we have no way to detect if mic routes are disabled right now
		// I don't know if this is even needed (power usage?)
		// dwNewPath = ES310_PATH_SUSPEND;
		// goto route;
	}
	// 1mic mode
	property_get("persist.audio.vns.mode", cVNRMode, "2");
	if (!strncmp("1", cVNRMode, 1))
		VNRMode = 1;
	else
		VNRMode = 2;

	if (mMode == AUDIO_MODE_IN_CALL || mMode == AUDIO_MODE_IN_COMMUNICATION) {
		// this needs a modification in platform.c, otherwise non-voice ucm rules will be selected
		bool is_voip = (mMode == AUDIO_MODE_IN_COMMUNICATION);

		switch (in_snd_device) {
		// speaker, hdmi
		case SND_DEVICE_IN_SPEAKER_MIC:
		case SND_DEVICE_IN_SPEAKER_MIC_AEC:
		case SND_DEVICE_IN_VOICE_SPEAKER_MIC:
#ifdef AUDIO_CAF
		case SND_DEVICE_IN_VOICE_SPEAKER_DMIC:
		case SND_DEVICE_IN_VOICE_SPEAKER_QMIC:
#else
		case SND_DEVICE_IN_VOICE_SPEAKER_DMIC_EF:
		case SND_DEVICE_IN_VOICE_SPEAKER_DMIC_BS:
#endif
		case SND_DEVICE_IN_HDMI_MIC:
			dwNewPath = ES310_PATH_HANDSFREE;
			dwNewPreset =
			    is_voip ? ES310_PRESET_HANDSFREE_VOIP_WB :
			    ES310_PRESET_HANDSFREE_INCALL_NB;
			break;

		// headset
		case SND_DEVICE_IN_HEADSET_MIC:
		// case SND_DEVICE_IN_HEADSET_MIC_AEC:
		case SND_DEVICE_IN_VOICE_HEADSET_MIC:
		case SND_DEVICE_IN_VOICE_TTY_FULL_HEADSET_MIC:
		case SND_DEVICE_IN_VOICE_TTY_HCO_HEADSET_MIC:
			dwNewPath = ES310_PATH_HEADSET;
			dwNewPreset =
			    is_voip ? ES310_PRESET_HEADSET_VOIP_WB :
			    ES310_PRESET_HEADSET_INCALL_NB;
			break;

		// handset, dmic, voice-rec, bluetooth
		case SND_DEVICE_IN_HANDSET_MIC:
		case SND_DEVICE_IN_HANDSET_MIC_AEC:
		case SND_DEVICE_IN_VOICE_TTY_VCO_HANDSET_MIC:
#ifdef AUDIO_CAF
		case SND_DEVICE_IN_VOICE_DMIC:
#else
		case SND_DEVICE_IN_VOICE_DMIC_EF:
		case SND_DEVICE_IN_VOICE_DMIC_BS:
		case SND_DEVICE_IN_VOICE_DMIC_EF_TMUS:
#endif
		case SND_DEVICE_IN_VOICE_REC_MIC:
#ifdef AUDIO_CAF
		// case SND_DEVICE_IN_VOICE_REC_DMIC:
		case SND_DEVICE_IN_VOICE_REC_DMIC_FLUENCE:
#else
		case SND_DEVICE_IN_VOICE_REC_DMIC_EF:
		case SND_DEVICE_IN_VOICE_REC_DMIC_BS:
		case SND_DEVICE_IN_VOICE_REC_DMIC_EF_FLUENCE:
		case SND_DEVICE_IN_VOICE_REC_DMIC_BS_FLUENCE:
#endif
		case SND_DEVICE_IN_BT_SCO_MIC:
		case SND_DEVICE_IN_BT_SCO_MIC_WB:
		default:
			dwNewPath = ES310_PATH_HANDSET;
			dwNewPreset =
			    is_voip ? ES310_PRESET_HANDSET_VOIP_WB :
			    ES310_PRESET_HANDSET_INCALL_NB;
			break;
		}
	} else {
		switch (in_snd_device) {
		// HDMI
		case SND_DEVICE_IN_HDMI_MIC:
			dwNewPath = ES310_PATH_HANDSET;
			dwNewPreset = ES310_PRESET_HANDSFREE_REC_WB;
			break;

		// bluetooth
		case SND_DEVICE_IN_BT_SCO_MIC:
		case SND_DEVICE_IN_BT_SCO_MIC_WB:
			dwNewPath = ES310_PATH_HEADSET;
			dwNewPreset = ES310_PRESET_HANDSFREE_REC_WB;
			break;

		// voice-rec-*
		case SND_DEVICE_IN_VOICE_REC_MIC:
#ifdef AUDIO_CAF
		// case SND_DEVICE_IN_VOICE_REC_DMIC:
		case SND_DEVICE_IN_VOICE_REC_DMIC_FLUENCE:
#else
		case SND_DEVICE_IN_VOICE_REC_DMIC_EF:
		case SND_DEVICE_IN_VOICE_REC_DMIC_BS:
		case SND_DEVICE_IN_VOICE_REC_DMIC_EF_FLUENCE:
		case SND_DEVICE_IN_VOICE_REC_DMIC_BS_FLUENCE:
#endif
			dwNewPath = ES310_PATH_HANDSET;
			dwNewPreset = ES310_PRESET_VOICE_RECOGNIZTION_WB;
			break;

		// headset-mic
		case SND_DEVICE_IN_HEADSET_MIC:
		// case SND_DEVICE_IN_HEADSET_MIC_AEC:
			dwNewPath = ES310_PATH_HEADSET;
			dwNewPreset = ES310_PRESET_HEADSET_MIC_ANALOG_BYPASS;
			break;

		// handset-mic, camcorder-mic, speaker-mic
		case SND_DEVICE_IN_HANDSET_MIC:
		case SND_DEVICE_IN_HANDSET_MIC_AEC:
		case SND_DEVICE_IN_CAMCORDER_MIC:
		case SND_DEVICE_IN_SPEAKER_MIC:
		case SND_DEVICE_IN_SPEAKER_MIC_AEC:
		default:
			dwNewPath = ES310_PATH_HANDSET;
			dwNewPreset = ES310_PRESET_ANALOG_BYPASS;
			break;
		}
	}

route:
	if (VNRMode == 1) {
		ALOGV("%s: Switch to 1-Mic Solution", __func__);
		if (dwNewPreset == ES310_PRESET_HANDSET_INCALL_NB) {
			dwNewPreset = ES310_PRESET_HANDSET_INCALL_NB_1MIC;
		}
		if (dwNewPreset == ES310_PRESET_HANDSET_VOIP_WB) {
			dwNewPreset = ES310_PRESET_HANDSET_INCALL_VOIP_WB_1MIC;
		}
	}
	// still the same path and preset
	if (dwOldPath == dwNewPath && dwOldPreset == dwNewPreset) {
		rc = 0;
		goto unlock;
	}

	ALOGD("%s: path=%s->%s, preset=%s->%s", __func__,
	      es310_getNameByPathID(dwOldPath),
	      es310_getNameByPathID(dwNewPath),
	      es310_getNameByPresetID(dwOldPreset),
	      es310_getNameByPresetID(dwNewPreset));

	// wakeup if suspended
	if (dwOldPath == ES310_PATH_SUSPEND) {
		rc = es310_wakeup(dev);
		if (rc) {
			ALOGE("%s: es310_wakeup() failed rc=%d\n", __func__,
			      rc);
			goto recover;
		}
	}
	// path
	if (dwOldPath != dwNewPath) {
		int retry = 4;
		do {
			ALOGE("%s: ioctl ES310_SET_CONFIG newPath:%d, retry:%d",
			      __func__, dwNewPath, (4 - retry));
			rc = ioctl(dev->mES310Fd, ES310_SET_CONFIG, &dwNewPath);

			if (!rc) {
				dev->dwPath = dwNewPath;
				break;
			} else {
				ALOGE("%s: ES310_SET_CONFIG rc=%d", __func__,
				      rc);
			}
		} while (--retry);

		if (rc)
			goto recover;
	}
	// preset
	if (dwNewPath != ES310_PATH_SUSPEND && (dwOldPreset != dwNewPreset)) {
		int retry = 4;
		do {
			ALOGE
			    ("%s: ioctl ES310_SET_PRESET newPreset:0x%x, retry:%d",
			     __func__, dwNewPreset, (4 - retry));
			rc = ioctl(dev->mES310Fd, ES310_SET_PRESET, &dwNewPreset);

			if (!rc) {
				dev->dwPreset = dwNewPreset;
				break;
			} else {
				ALOGE("%s: ES310_SET_PRESET rc=%d", __func__,
				      rc);
			}
		} while (--retry);

		if (rc)
			goto recover;
	}

recover:
	if (rc < 0) {
		ALOGE("%s: ES310 do hard reset to recover from error!\n",
		      __func__);

		// close device first
		close(dev->mES310Fd);
		dev->mES310Fd = -1;

		// re-init
		rc = es310_init(dev);
		if (!rc) {
			// set new config
			rc = ioctl(dev->mES310Fd, ES310_SET_CONFIG, &dwNewPath);
			if (rc) {
				ALOGE("%s: RECOVERY: ES310_SET_CONFIG rc=%d",
				      __func__, rc);
				goto unlock;
			}

			dev->dwPath = dwNewPath;
		} else {
			ALOGE("%s: RECOVERY: es310_init() failed rc=%d\n",
			      __func__, rc);
			goto unlock;
		}
	}

unlock:
	pthread_mutex_unlock(&dev->mES310Mutex);
	return rc;
}

static int amplifier_init(amp_device_t *dev)
{
	dev->mode = AUDIO_MODE_NORMAL;
	dev->mES310Fd = -1;
	dev->dwPath = ES310_PATH_SUSPEND;
	dev->dwPreset = -1;
	dev->in_snd_device = SND_DEVICE_NONE;
	dev->out_snd_device = SND_DEVICE_NONE;

	// mutex init
	pthread_mutex_init(&dev->mES310Mutex, NULL);

	return es310_init(dev);
}

static int set_input_devices(struct amplifier_device *device, uint32_t snd_device) {
	amp_device_t *dev = (amp_device_t *) device;
	snd_device_t new_in_snd_device = SND_DEVICE_NONE;

	if (snd_device != 0) {
		new_in_snd_device = snd_device;

		if (new_in_snd_device != dev->in_snd_device) {
			dev->in_snd_device = new_in_snd_device;

			es310_do_route(dev);
		}
	}

	return 0;
}

static int set_output_devices(struct amplifier_device *device, uint32_t snd_device) {
	amp_device_t *dev = (amp_device_t *) device;
	snd_device_t new_out_snd_device = SND_DEVICE_NONE;
	
	if (snd_device != 0) {
		new_out_snd_device = snd_device;

		if (new_out_snd_device != dev->out_snd_device) {
			dev->out_snd_device = new_out_snd_device;

			es310_do_route(dev);
		}
	}

	return 0;
}

static int amplifier_set_mode(struct amplifier_device *device, audio_mode_t mode) {
	amp_device_t *dev = (amp_device_t *) device;
	int rc = -1;
	
	if ((mode < AUDIO_MODE_CURRENT) || (mode >= AUDIO_MODE_CNT)) {
		ALOGW("%s: invalid mode=%d\n", __func__, mode);
		return rc;
	}

	if (dev->mode != mode) {
		ALOGD("%s: mode: %d->%d\n", __func__, dev->mode, mode);
		dev->mode = mode;
		es310_do_route(dev);
	}

	return 0;
}

static int amp_dev_close(hw_device_t *device)
{
    amp_device_t *dev = (amp_device_t *) device;

		close(dev->mES310Fd);
		dev->mES310Fd = -1;
    free(dev);

    return 0;
}

static int amp_module_open(const hw_module_t *module, const char *name UNUSED,
        hw_device_t **device)
{
	int ret;

	if (amp_dev) {
		ALOGE("%s:%d: Unable to open second instance of the amplifier\n", __func__, __LINE__);
		return -EBUSY;
	}

	amp_dev = calloc(1, sizeof(amp_device_t));
	if (!amp_dev) {
		ALOGE("%s:%d: Unable to allocate memory for amplifier device\n", __func__, __LINE__);
		return -ENOMEM;
	}

	amp_dev->amp_dev.common.tag = HARDWARE_DEVICE_TAG;
	amp_dev->amp_dev.common.module = (hw_module_t *) module;
	amp_dev->amp_dev.common.version = HARDWARE_DEVICE_API_VERSION(1, 0);
	amp_dev->amp_dev.common.close = amp_dev_close;

	amp_dev->amp_dev.set_input_devices = set_input_devices;
	amp_dev->amp_dev.set_output_devices = set_output_devices;
	amp_dev->amp_dev.enable_input_devices = NULL;
	amp_dev->amp_dev.enable_output_devices = NULL;
	amp_dev->amp_dev.set_mode = amplifier_set_mode;
	amp_dev->amp_dev.output_stream_start = NULL;
	amp_dev->amp_dev.input_stream_start = NULL;
	amp_dev->amp_dev.output_stream_standby = NULL;
	amp_dev->amp_dev.input_stream_standby = NULL;

	amplifier_init(amp_dev);
	*device = (hw_device_t *) amp_dev;

	return 0;
}

static struct hw_module_methods_t hal_module_methods = {
	.open = amp_module_open,
};

amplifier_module_t HAL_MODULE_INFO_SYM = {
	.common = {
			.tag = HARDWARE_MODULE_TAG,
			.module_api_version = AMPLIFIER_MODULE_API_VERSION_0_1,
			.hal_api_version = HARDWARE_HAL_API_VERSION,
			.id = AMPLIFIER_HARDWARE_MODULE_ID,
			.name = "Xiaomi aries amplifier HAL",
			.author = "The LineageOS Open Source Project",
			.methods = &hal_module_methods,
	},
};
