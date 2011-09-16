#! /bin/bash -xe

#DOX=--generate-doxygen

xsdcxx cxx-tree --generate-serialization $DOX sshshare.xsd
xsdcxx cxx-tree --generate-serialization $DOX sshshare_config.xsd

