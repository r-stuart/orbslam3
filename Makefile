build:
	DOCKER_BUILDKIT=1 docker build -f Dockerfile -t orbslam_base .

debug:
	docker run --rm -it --entrypoint /bin/bash -v /mnt/slow:/mh orbslam_base

run:
	mkdir -p out && \
	python producer.py | docker run -i --rm -v $(shell pwd)/out:/out orbslam_base 3682 
