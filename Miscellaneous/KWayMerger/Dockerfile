FROM golang:latest
LABEL maintainer="Haohu Shen"
ADD . /KWayMerger
WORKDIR /KWayMerger
ENTRYPOINT ["go", "test", "./test", "-v"]
