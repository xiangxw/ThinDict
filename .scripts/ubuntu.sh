#!/bin/bash

# for example: 0.1.1
THINDICT_VERSION=$(awk 'NR == 1 {print substr($2, 2, index($2, "-") - 2)}' debian/changelog)
# for example: 0.1.1-1
THINDICT_PACKAGE_VERSION=$(awk 'NR == 1 {print substr($2, 2, length($2) - 2)}' debian/changelog)
# ubuntu series
UBUNTU_SERIES=('precise' 'quantal' 'raring' 'saucy' 'trusty')
# name
NAME='xiangxw'
# email
EMAIL='xiangxw5689@126.com'

function createOriginSource()
{
	DEBFULLNAME=${NAME} dh_make -c lgpl3 --email ${EMAIL} --createorig -p thindict_${THINDICT_VERSION} --single --yes
}

function debuildBinary()
{
	# build binary package
	debuild
}

function debuildSource()
{
	# build source package
	echo 'thindict ('${THINDICT_PACKAGE_VERSION}'ubuntu1ppa1~'$1'1) '$1'; urgency=low' > tmp
	echo '' >> tmp
	echo '  * For ubuntu '$1' ppa' >> tmp
	echo '' >> tmp
	echo ' -- '${NAME}' <'${EMAIL}'>  '`LANG=C date -R` >> tmp
	echo '' >> tmp
	cat debian/changelog >> tmp
	mv tmp debian/changelog
	debuild -S -sa -kxiangxw5689@126.com
	git checkout -- debian/changelog
}

function debuildAllSourceAndUpload()
{
	# build source packages for all ubuntu series and upload them
	for (( i = 0; i < ${#UBUNTU_SERIES[@]}; i++ )); do
		debuildSource ${UBUNTU_SERIES[$i]}
	done
	dput ppa:xiangxw5689/thindict ../thindict_*_source.changes
}

# remove old packages
rm ../thindict*
# create origin source package
createOriginSource
# build
if [[ $# -eq 0 ]]; then
	debuildAllSourceAndUpload
elif [[ "$1" == "binary" ]]; then
	debuildBinary
fi
