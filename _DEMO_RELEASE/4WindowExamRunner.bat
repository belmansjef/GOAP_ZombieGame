start GPP_TEST_RELEASE.exe -x 0 -y 30 -s 423

start GPP_TEST_RELEASE.exe -x 850 -y 30 -s 126

start GPP_TEST_RELEASE.exe -x 0 -y 525 -s 149

start GPP_TEST_RELEASE.exe -x 850 -y 525 -s 324

echo new ActiveXObject("WScript.Shell").AppActivate("GPP_TEST_RELEASE.exe"); > tmp.js
cscript //nologo tmp.js & del tmp.js