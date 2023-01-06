start GPP_TEST_RELEASE.exe -x 0 -y 30 -s 15

start GPP_TEST_RELEASE.exe -x 850 -y 30 -s 9

start GPP_TEST_RELEASE.exe -x 0 -y 525 -s 3

start GPP_TEST_RELEASE.exe -x 850 -y 525 -s 18

echo new ActiveXObject("WScript.Shell").AppActivate("GPP_TEST_RELEASE.exe"); > tmp.js
cscript //nologo tmp.js & del tmp.js