import os, sys, time
import cv2
import subprocess
import random
import argparse

class PersonDescriptor:
	def __init__(self,img_path,img_size,bounding_box,desc):
		if not (img_path or img_size or bounding_box):
			return Exception("Argument(s) to PersonDescriptor must have valid data")
		else:
			self.img_path = img_path
			self.img_size = img_size
			self.BB = bounding_box 
			self.desc = desc

def process_single_sample(path,root_dir):
	try:
		fp = open(path,"r")
	except IOError, e:
		return None
	
	lines = map(lambda x: x.rstrip(),  fp.readlines())

	img_path = "" 
	img_size = None 
	BB = None
	desc = ""
	
	for line in lines:
		if "Image filename" in line:
			img_path = root_dir+line.split(":")[1].strip().rstrip()[1:-1]

		if "Image size" in line:
			# FORMAT key : width x height x colors
			img_size = map(int,line.split(":")[1].split("x"))

		if "Bounding box" in line:
			# FORMAT key : (Xmin,Ymin) - (Xmax,Ymax)
			two_points = line.split(":")[1].split("-")
			p1 = map(int,two_points[0].strip().rstrip()[1:-1].split(","))
			p2 = map(int,two_points[1].strip().rstrip()[1:-1].split(","))
			BB = [p1,p2]

		if "Original label" in line:
			desc = line.split(":")[1].strip().rstrip()[1:-1]

	return PersonDescriptor(img_path,img_size,BB,desc)


def process_all_training_samples(annotated_file_list, prog, root_dir, neg, verbose=False):
	PD_list = [] 
	
	fp = open("svm_pos.txt","w+")

	if verbose:	
		t1 = time.time()
		total_time = t1
		ticker = 0

	for path in annotated_file_list:
		PDo = process_single_sample(path,root_dir)

		if PDo:
			PD_list.append(PDo)
			cmd = map(str,[prog, PDo.img_path,PDo.BB[0][0],PDo.BB[0][1],PDo.BB[1][0],PDo.BB[1][1]])
			proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
			
			hist = proc.stdout.readlines()
			if len(hist) > 0:
				hist = hist[0][:-1]
				fp.write("+1 "+hist+"#"+PDo.img_path+"\n")
			else:
				print "Failed to run",PDo.img_path
				print "ERROR: ",hist

			if verbose:
				ticker += 1
				if ticker > 10:
					print "Processing speed: ",(time.time()-t1)/ticker," samples / sec"
					ticker = 0
					t1 = time.time()

			

	fp.close()
	if verbose:
		print "Total time: ",time.time()-total_time," sec"

	return PD_list




def train_negative(image_path, prog):

	neg_image_path = map(lambda filename: image_path+filename, next(os.walk(image_path))[2])

	fp = open("svm_neg.txt","w+")

	for neg_img in neg_image_path:
		img = cv2.imread(neg_img)

		height, width, depth = img.shape

		start_x = random.randint(1,width-65)
		start_y = random.randint(1,height-129)

		cmd = map(str,[prog, neg_img, start_x, start_y, start_x+64, start_y+128])
		proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
		hist = proc.stdout.readlines()

		hist = hist[0].strip().rstrip()[1:-1]
		fp.write("-1 "+hist+"# "+neg_img+"\n")

	fp.close()

if __name__ == "__main__":
	usage = ""
	parser = argparse.ArgumentParser(description='Train an SVM using SVM light') 
	parser.add_argument("-p", "--path", dest="path", type=str, help="The path to image database annotations")
	parser.add_argument("-n", "--negative", dest="neg", type=str, help="Set these samples to negative samples. Positive is the default")
	parser.add_argument("-v", "--verbose", dest="verbose", help="Enable verbose output", action="store_true")
	parser.add_argument("-r", "--root_dir", dest="root_dir", type=str, help="Sets the root folder of test DB")
	parser.add_argument("--prog", dest="program", type=str, help="Perform this program on every image described by the annotations")

	args = parser.parse_args()

	fp = open("svm_config.txt","w")
	fp.close()

	if args.path and args.program:
	
		if args.verbose:
			print "="*40
			print "SVM light trainer wrapper"
			print "="*40


		annotated_file_list = map(lambda filename: args.path+filename, next(os.walk(args.path))[2])

		if args.verbose:
			print "Loaded: ",len(annotated_file_list)," files"

		PD_list = process_all_training_samples(annotated_file_list, args.program, args.root_dir, args.neg, args.verbose)

		if args.verbose :
			print "Parsing DONE ( %d / %d ) successfully parsed" % (len(PD_list), len(annotated_file_list))
		
	if args.neg and args.program:
		train_negative( args.neg, args.program )


