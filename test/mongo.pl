use strict;
use warnings;

use MongoDB;
use MongoDB:OID;


my $client = MongoDB::MongoClient->new;

my $db = $client->get_database('mydb');

my $employees = $db->get_collection("employees");

my $i;
my $age;
my $salary;
for($i =0; $i<100; $i++) {
	$age = $i * 5 + $i;
	$salary = $i * 1000;
	$employees->insert("name"=>"sahil aggarwal",
			   "salary"=> $salary,
			   "age"=> $age);
}
