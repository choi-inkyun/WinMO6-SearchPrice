
#pragma once

#pragma comment(lib, "wininet.lib")

typedef enum
{
	LIST_SELECT = 2001,				/* ����Ʈ �ڽ����� � ���� ���� �Ǿ��� �� */
	UNLOAD_LIST,					/* ����Ʈ �ڽ����� ���� ���̰� ���� �ʴ� ������ �����ش� */

	IMAGEFILE_DOWNLOADING,			/* �̹����� ������� �ٿ�޴� �߿� ��Ȳ�� �����Ѵ� */
	IMAGEFILE_DOWNLOAD_COMPLETE,	/* ����Ʈ ��Ʈ�ѿ��� ������� �̹��� ������ �ٿ�ε� �ϰ� ���� �����ش� */
} UIEVENT;

#define IMAGE_DOWNLOAD_WAIT_TIMER 100

#define DEFAULT_DOWNLOAD_SETUP_CONF			L"GoodsDataTemp.xml"