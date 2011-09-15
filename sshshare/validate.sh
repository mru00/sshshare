#! /bin/bash -xe


if which xmlstarlet; then
  exec xmlstarlet val -e -s sshshare.xsd sharedata.xml
fi


if which xmllint; then
  exec xmllint --noout --valid --schema sshshare.xsd sharedata.xml
fi

echo "no validation software found"
exit 1

