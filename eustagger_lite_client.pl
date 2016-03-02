#!/usr/bin/perl -w

use IO::Socket;

##client script for sending files to eustagger_lite server
##usage:  /eustagger_lite_client.pl [port] [absolute_path_of_file]

my $port=shift;
my $filepath=shift;

$socket = new IO::Socket::INET (
    PeerAddr  => 'localhost',
    PeerPort  =>  $port,
    Proto => 'tcp',
)
or die "Couldn't connect to Server\n";


$socket->send($filepath);
shutdown($socket, 1);


while ($line = <$socket>) 
{
    print $line;
}
 
close($socket);
