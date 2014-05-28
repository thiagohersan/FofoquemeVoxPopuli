# run VoxPopuli python
isrun=`ps -u root | grep python | wc -l`
if [ $isrun -lt 1 ]
then
    cd /home/pi/Dev/FofoquemeVoxPopuli/Python/VoxPopuli
    http-server -p 8666 &
    while [ 1 -le 20 ]
    do
	sudo python VoxPopuli.py &
	sleep 1
	killsudopid=$!
	killpythonpid=`ps -u root | awk '/python/{print $1}'`
	sleep 600
	sudo kill -9 $killpythonpid
	sudo kill -9 $killsudopid
    done
fi
