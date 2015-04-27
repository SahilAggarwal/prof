#!/usr/bin/perl

use DBI;
use strict;
use warnings;
use Time::HiRes qw(time);

my $driver = "SQLite";
my $databse = "test.db";
my $dsn = "DBI:$driver:dbname=$databse";

my $userid = "";
my $password = "";

my $dbh = DBI->connect($dsn, $userid, $password, { RaiseError => 1}) or die $DBI::errstr;

print "Opened successfully\n";

my $query;
my $rv;
eval {	
	$query  = qq(DROP TABLE COMPANY);
	$rv =$dbh->do($query);
	if($rv < 0){
		print $DBI::errstr;
	} else {
		print "Cleaned the DB\n";
	}
};

$query = qq(CREATE TABLE COMPANY 
	       (ID INT PRIMARY KEY NOT NULL,
     		NAME TEXT NOT NULL,
		AGE INT NOT NULL,
		SALARY REAL););

 $rv =$dbh->do($query);
if($rv < 0){
	print $DBI::errstr;
} else {
   	print "Table created successfully\n";
}

my $i = 1;

my $total_time = 0;

for(; $i<=100; $i++) {
	my $age = $i + 10;
	my $salary = $i*100;
	$query = qq(INSERT INTO COMPANY (ID,NAME,AGE,SALARY
		   ) VALUES($i,"sahil",$age,$salary));

	my $start = time();
	$rv = $dbh->do($query) or die $DBI::errstr;
	my $end = time();
	
	$total_time += $end - $start;
}

print "Records created successfully\n";

print "Average Time:".$total_time;

$dbh->disconnect;
