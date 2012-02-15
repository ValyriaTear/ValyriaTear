rm en@quot -rf
make en@quot.mo
mkdir en@quot
mkdir en@quot/LC_MESSAGES
mv en@quot.mo en@quot/LC_MESSAGES/allacrost.mo

rm fr -rf
make fr.mo
mkdir fr
mkdir fr/LC_MESSAGES
mv fr.mo fr/LC_MESSAGES/allacrost.mo

rm es -rf
make es.mo
mkdir es
mkdir es/LC_MESSAGES
mv es.mo es/LC_MESSAGES/allacrost.mo

rm pt_BR -rf
make pt_BR.mo
mkdir pt_BR
mkdir pt_BR/LC_MESSAGES
mv pt_BR.mo pt_BR/LC_MESSAGES/allacrost.mo

rm de -rf
make de.mo
mkdir de
mkdir de/LC_MESSAGES
mv de.mo de/LC_MESSAGES/allacrost.mo
