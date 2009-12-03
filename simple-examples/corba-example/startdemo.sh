
echo "Compiling..."
make eserver eclient || exit 1

echo "Starting Naming_Service..."
#try to start naming service
(Naming_Service -m 1 &) || (Naming_Service -m 1 &) || exit 1

echo "Starting server..."
./eserver &

echo "Waiting...10s"
sleep 10

echo "Starting client..."
./eclient

echo "Killing left-overs..."
killall eserver
killall Naming_Service