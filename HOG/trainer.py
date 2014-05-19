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
			self.scale_w =  64.0/(bounding_box[1][0]-bounding_box[0][0])
			self.scale_h = 128.0/(bounding_box[1][1]-bounding_box[0][1])


def process_single_sample(path,root_dir):
	try:
		fp = open(path+"","r")
	except IOError, e:
		print "ERROR: could not open",path
		return None
	
	lines = map(lambda x: x.rstrip(),  fp.readlines())

	img_path = "" 
	img_size = None 
	BB = None
	desc = ""

	persons = []
	
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

		if img_path!=None and img_size!=None and BB!=None and desc!=None:
			persons.append( PersonDescriptor(img_path,img_size,BB,desc) )
			img_path = ""
			img_size = None
			BB = None
			desc = ""

	return persons

def process_cropped(file_list, prog, root_dir, neg, verbose=False):
	fp = open("svm_pos.txt","w")

	num_samples = len(file_list)
	num_done = 1

	for path in file_list:
		cmd = [prog,path,"0","0", "64","128"]
		proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)

		HOG_output = proc.stdout.readlines()
		if len(HOG_output) > 0:
			HOG_output= HOG_output[0][:-1]
			fp.write("+1 "+HOG_output+" # "+path+"\n")
		else:
			print "Failed to run",path
			print "ERROR: ",HOG_output

		print num_done,"/", num_samples 
		num_done += 1
	

	fp.close()

def process_all_training_samples(annotated_file_list, prog, root_dir, neg, verbose=False):
	PD_list = [] 
	
	fp = open("svm_pos.txt","w+")

	if verbose:	
		t1 = time.time()
		total_time = t1
		ticker = 0

	for path in annotated_file_list:
		PDo_sublist = process_single_sample(path,root_dir)

		if PDo_sublist:
			for PDo in PDo_sublist:
				if PDo:
					PD_list.append(PDo)
					cmd = map(str,[prog, PDo.img_path,PDo.BB[0][0],PDo.BB[0][1],PDo.BB[1][0],PDo.BB[1][1]])
					print cmd
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

	train_width = 64 
	train_height = 128 

	counter = 1 

	for neg_img in neg_image_path:
		img = cv2.imread(neg_img)

		height, width, depth = img.shape

		if width-train_width-1 > 0 and height - train_height-1> 0:
			for x in range(0,8):
				start_x = random.randint(1,width-train_width-1)
				start_y = random.randint(1,height-train_height-1)

				cmd = map(str,[prog, neg_img, start_x, start_y, start_x+train_width, start_y+train_height])
				proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
				hist = proc.stdout.readlines()

				if len(hist) > 0:
					hist = hist[0].strip().rstrip()[1:-1]
					fp.write("-1 "+hist+" # "+neg_img+"\n")
				else:
					"Print failed to read:",neg_img
		print counter,"/",len(neg_image_path)
		counter += 1	

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

		process_cropped(annotated_file_list, args.program, args.root_dir, args.neg, args.verbose)
		#PD_list = process_all_training_samples(annotated_file_list, args.program, args.root_dir, args.neg, args.verbose)

		if args.verbose :
			print "Parsing DONE ( %d / %d ) successfully parsed" % (len(PD_list), len(annotated_file_list))
		
	if args.neg and args.program:
		train_negative( args.neg, args.program )


