while [ 1 ] ; do
sudo tunslip/tunslip6 -a 127.0.0.1 -p 60001 -v2 fd00::1/64
sleep 5
done
