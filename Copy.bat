// 커맨드 명령어				옵션				원본 파일이 있는 위치			사본 파일을 저장할 위치

xcopy			/y				.\Engine\Header\*.*						.\Reference\Header\

xcopy			/y				.\Engine\System\Bin\System.dll			.\Client\Bin\
xcopy			/y				.\Engine\Utility\Bin\Utility.dll		.\Client\Bin\

xcopy			/y				.\Engine\System\Bin\System.dll			.\Tool\Bin\
xcopy			/y				.\Engine\Utility\Bin\Utility.dll		.\Tool\Bin\

xcopy			/y				.\Engine\System\Bin\System.lib			.\Reference\Library\
xcopy			/y				.\Engine\Utility\Bin\Utility.lib		.\Reference\Library\

xcopy			/d /e /y			.\Resource\Texture					.\Client\Bin\Resource\Texture\
xcopy			/d /e /y			.\Resource\Sound					.\Client\Bin\Resource\Sound\

xcopy			/d /e /y			.\Resource\FMOD						.\Engine\System\Bin\
xcopy			/d /e /y			.\Resource\FMOD						.\Client\Bin\