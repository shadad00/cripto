# (k, n) secret sharing scheme capable of cheating detection

## Introduction
This code implements a secret sharing scheme as described in:
https://jwcn-eurasipjournals.springeropen.com/articles/10.1186/s13638-018-1084-7. 
In this case, k is provided as parameter while n is equal to 8. The secret is a *.bmp file, and is hidden into n .bmp files. 


## Running 🛠️
Clone the repository
```bash
git clone https://github.com/shadad00/cripto.git
```
Build the executable from the root of the project.
```bash
make all
```



## Distribute 🚀
A secret, k and the images where the secret will be hidden must be provided. 
```bash
./ss d secret k path-to-the-images
```

## Recover 🚀
The k and the images to rebuild the secret must be provided. The reconstruction will be in the file named 'generated-file'.
```bash
./ss d generated-file k path-to-the-images
```



## Made by: 💭
- [Gaspar Budó Berra](https://github.com/gbudoberra)
- [Santiago Hadad](https://github.com/shadad00)
- [Bruno Squillari](https://github.com/bsquillari)
- [Facundo Zimbimbakis](https://github.com/fzimbimbakis)
