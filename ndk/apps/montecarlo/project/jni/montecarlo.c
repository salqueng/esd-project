#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/mman.h>
#include <errno.h>
#include <android/log.h>

jint
Java_esd2014_montecarlo_team5_BlackScholesActivity_MCWrite(JNIEnv* env, jobject
		this, jstring data) {
	int dev, ret;
	dev = open("/dev/montecarlo", O_RDWR|O_SYNC);
	char *result = (*env) -> GetStringUTFChars(env, data, NULL);
	
	if (dev != -1) {
		ret = write(dev, result, strlen(result)+1);
		close(dev);
	} else {
		__android_log_print(ANDROID_LOG_ERROR, "BlackScholesActivity", "Device Open ERROR!\n");
		exit(1);
	}
	return 0;
}

jint
Java_esd2014_montecarlo_team5_BlackScholesActivity_MCIOControl(JNIEnv* env, jobject this, jint data) {
	int dev, ret;
	dev = open("/dev/montecarlo", O_RDWR|O_SYNC);

	if (dev != -1) {
		ret = ioctl(dev, data, NULL, NULL);
		close(dev);
	} else {
		__android_log_print(ANDROID_LOG_ERROR, "BlackScholesActivity", "Device Open ERROR!\n");
		exit(1);
	}
	return 0;
}

jstring
Java_esd2014_montecarlo_team5_BlackScholesActivity_MCRead(JNIEnv* env, jobject this) {
	int dev, ret;
	jstring data;
	dev = open("/dev/montecarlo", O_RDONLY);

	if (dev != -1) {
		ret = read(dev, &data, 4);
		close(dev);
	} else {	
		__android_log_print(ANDROID_LOG_ERROR, "BlackScholesActivity", "Device Open ERROR!\n");
		exit(1);
	}
	return data;
}
