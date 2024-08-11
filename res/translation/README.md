code:bash
./Projucer.exe --trans ./src > ./res/translation/japanese.template.txt
cp ./res/translation/japanese.template.txt ./res/translation/japanese.work.txt
vim ./res/translation/japanese.work.txt
./Projucer.exe --trans-finish ./res/translation/japanese.template.txt ./res/translation/japanese.work.txt ./res/translation/japanese.txt > ./res/translation/japanese.next.txt
vim ./res/translation/japanese.next.txt
cp ./res/translation/japanese.next.txt ./res/translation/japanese.txt
