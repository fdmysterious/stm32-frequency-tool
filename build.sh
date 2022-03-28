PWD=`pwd`

mkdir -p output
mkdir -p build

docker run --rm -it \
	--mount type=bind,src=${PWD}/scripts,dst=/scripts \
	--mount type=bind,src=${PWD}/project,dst=/project \
	--mount type=bind,src=${PWD}/output,dst=/output   \
	--mount type=bind,src=${PWD}/build,dst=/build     \
	docker-stm32 "/scripts/build.sh"
