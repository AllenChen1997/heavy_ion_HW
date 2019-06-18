# heavy_ion_HW

# how to use

	git clone https://github.com/AllenChen1997/heavy_ion_HW.git
	cd heavy_ion_HW/
	root -l
	.L my_draw.C
	my_draw("data.txt")

you will see the plot on screen and the saved data canvas.png.

make sure the data.txt is following the form. the second line number is signn or xsect.


# about the my_draw.C

inside the file. L11 to L14 can be modified

maxpt is the entries you want in L1L2 plot

if mode = 1, shows the last time run result( maxpt can set smaller )

if mose = 0, plot the L1L2 plot

B is impact para, fR is the radius of the particle
