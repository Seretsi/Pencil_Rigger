

def fix_OBJ(filename):
	objfile = open(filename).readlines()
	lines_to_delete = []
	new_face_lines = []
	new_face_lines.append('\n')
	for line in range(0,len(objfile)):
		if (objfile[line] == '' or objfile[line] == '\n'):
			lines_to_delete.append(line)
			continue
		words = objfile[line].split()
		if (len(words) == 0):
			continue
		if (words[0] == 'v'):
			continue
		elif (words[0] == 'vt' or words[0] == 'vn'):
			lines_to_delete.append(line)
		elif words[0] == 'f':
			#parse with 4 vertices
			fixed_line = ""
			fixed_line += "f "
			for i in range(1,5):
				words[i] = words[i].split('/')
				fixed_line += words[i][0] + " "
			fixed_line += "\n"
			new_face_lines.append(fixed_line)
			lines_to_delete.append(line)
		else:
			lines_to_delete.append(line)
	for num in lines_to_delete:
		objfile[num]=''
	for new_line in new_face_lines:
		objfile.append(new_line)
	newfile = open('new_'+filename, 'w+')
	for line in objfile:
			newfile.write(line)
		
	newfile.close()


if __name__ == '__main__':
	filename = None
	while (filename == None):
		filename = raw_input("Enter an OBJ file to be \'fixed\': ")
		if (not filename.endswith('.obj')):
			filename = None
	newfilename = fix_OBJ(filename)





