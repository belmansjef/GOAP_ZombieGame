start GPP_TEST_RELEASE.exe -x 0 -y 30 -s 160
start GPP_TEST_RELEASE.exe -x 480 -y 30 -s 160
start GPP_TEST_RELEASE.exe -x 960 -y 30 -s 160
start GPP_TEST_RELEASE.exe -x 1240 -y 30 -s 160
start GPP_TEST_RELEASE.exe -x 0 -y 490 -s 160
start GPP_TEST_RELEASE.exe -x 480 -y 490 -s 160
start GPP_TEST_RELEASE.exe -x 960 -y 490 -s 160
start GPP_TEST_RELEASE.exe -x 1240 -y 490 -s 160
echo new ActiveXObject("WScript.Shell").AppActivate("GPP_TEST_RELEASE.exe"); > tmp.js
cscript //nologo tmp.js & del tmp.js