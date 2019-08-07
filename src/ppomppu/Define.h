
#pragma once

#pragma comment(lib, "wininet.lib")

typedef enum
{
	LIST_SELECT = 2001,				/* 리스트 박스에서 어떤 셀을 선택 되었을 때 */
	UNLOAD_LIST,					/* 리스트 박스에서 눈에 보이게 되지 않는 영역을 보내준다 */

	IMAGEFILE_DOWNLOADING,			/* 이미지를 쓰레드로 다운받는 중에 상황을 전송한다 */
	IMAGEFILE_DOWNLOAD_COMPLETE,	/* 리스트 콘트롤에서 쓰레드로 이미지 파일을 다운로드 하고 나서 보내준다 */
} UIEVENT;

#define IMAGE_DOWNLOAD_WAIT_TIMER 100

#define DEFAULT_DOWNLOAD_SETUP_CONF			L"GoodsDataTemp.xml"