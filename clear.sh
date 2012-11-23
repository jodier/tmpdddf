#!/bin/sh

rm -fr app/cache
rm -fr app/logs

mkdir app/cache
mkdir app/logs

chmod 777 app/cache
chmod 777 app/logs

php app/console cache:clear
php app/console cache:clear --env=prod

php app/console assets:install web

#chown web3:client1 -R app/cache
#chown web3:client1 -R app/logs
