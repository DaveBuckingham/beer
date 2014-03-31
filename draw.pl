#!/usr/bin/perl
use strict;
use warnings;
use GD::Image;

# VARIABLES
my $lineWidth = 1;
my $FOOD_SIZE = 20;
my $WIDTH = 800;;
my $HEIGHT = 800;
my $scale = 10;
my $h_shift = $WIDTH / 2;
my $v_shift = $HEIGHT / 2;
my $outFile = 'image.png';
my ($x_new, $y_new);
my ($x_bot, $y_bot);

sub project {
    my ($x_in, $y_in) = @_;
    return (($x_in * $scale) + $h_shift, ($y_in * $scale) + $v_shift);
}

my $im = new GD::Image($WIDTH, $HEIGHT);
$im->interlaced('false');
my $imBackColor = $im->colorAllocate(0, 0, 0);
#my $imLineColor = $im->colorAllocate(30, 80, 200);
#my $imFoodColor = $im->colorAllocate(0, 200, 0);
my @colors;
$colors[0] = $im->colorAllocate(200, 0, 0);
$colors[1] = $im->colorAllocate(0, 200, 0);
$colors[2] = $im->colorAllocate(0, 0, 200);
$im->fill(0, 0, $imBackColor);
$im->setThickness($lineWidth);

my $color_index = -1;

while (my $line = <STDIN>) {
    chomp($line);
    if ($line =~ m/^FOOD:/) {
	$line =~ s/FOOD: //;
	my ($x_food, $y_food) = project(split(' ', $line));
	$color_index++;
	$im->arc($x_food, $y_food, $FOOD_SIZE, $FOOD_SIZE, 0, 360, $colors[$color_index]);
	$im->arc($x_food, $y_food, $FOOD_SIZE / 10, $FOOD_SIZE / 10, 0, 360, $colors[$color_index]);
    }
    else {
	my ($x_new, $y_new) = project(split(' ', $line));
	if (defined($x_bot)) {
	    GD::Image::line($im, $x_bot, $y_bot, $x_new, $y_new, $colors[$color_index]);
	}
	($x_bot, $y_bot) = ($x_new, $y_new);
    }
}


# CONVERT IMAGE TO PNG AND PRINT TO FILE
open (FILEHANDLE, '>' . $outFile) || die "failed to create/open file: \"$outFile\"\n";
binmode FILEHANDLE; #for windows
print FILEHANDLE $im->png;
close (FILEHANDLE); 
