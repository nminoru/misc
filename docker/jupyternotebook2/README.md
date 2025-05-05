# build image
docker build -t classicnote .

# run container
docker run -p 18888:8888 -e GRANT_SUDO=yes --user root --name classicnote classicnote
