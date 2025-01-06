# Blank PD Object

## !!!DON'T CLONE THIS REPO!!!
### Download as a zip 
extract the contents 
rename the folder and configure your project

to configure the project run

```bash
  sh configure.sh <OBJECT NAME> <spectral>
```

Don't add a ~ after you object name.
Script will automaticall append a ~ to the object name to adhere to the pd naming convention

adding the word `spectral` after your object name will automatically build and include the spectral-dsp library
leave it blank if this isn't needed

make a folder called dev (this is ignored in .gitignore)

```bash
  mkdir dev
```

add your own build script to the ignored folder

```bash
  touch dev/build.sh
```

add the cmake commands and specify an install file path

```bash
  #!/bin/bash

  cmake . -B build -G "Unix Makefiles"
  cmake --build build
```


