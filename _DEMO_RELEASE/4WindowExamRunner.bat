<<<<<<< Updated upstream
start GPP_TEST_RELEASE.exe -x 30 -y 30 -s 1

start GPP_TEST_RELEASE.exe -x 1000 -y 30 -s 1

start GPP_TEST_RELEASE.exe -x 30 -y 540 -s 1
=======
start GPP_TEST_RELEASE.exe -x 0 -y 30 -s 1

start GPP_TEST_RELEASE.exe -x 850 -y 30 -s 2

start GPP_TEST_RELEASE.exe -x 0 -y 525 -s 3

start GPP_TEST_RELEASE.exe -x 850 -y 525 -s 324
>>>>>>> Stashed changes

start GPP_TEST_RELEASE.exe -x 1000 -y 540 -s 1
echo new ActiveXObject("WScript.Shell").AppActivate("GPP_TEST_RELEASE.exe"); > tmp.js
cscript //nologo tmp.js & del tmp.js