COMPONENT_NAME := WIFI_OTA
COMPONENT_OBJS := index.o wifi_ota.o
COMPONENT_ADD_INCLUDEDIRS := .

define embed_img_script
import base64
base64_image="NULL"
with open("../../wirrigator_logo.jpg") as img_file:
	base64_image = base64.b64encode(img_file.read()).decode("utf-8")
with open("index.html","r") as hfile:
	hcont=hfile.read()
	modhcont=hcont.replace('<img src=""', '<img src="data:image/jpeg;base64,{}"'.format(base64_image))
with open("index.html","wb") as hfile:
	hfile.write(modhcont)
endef

export embed_img_script

index.html:
	$(summary) "Copying $@ to current dir..."
	cp $(COMPONENT_PATH)/index.html .

index.o: index.html
	$(summary) "Embedding logo file..."
	echo "$$embed_img_script" > embed_img.py
	python embed_img.py
	$(summary) "Generating $@ file..."
	$(OBJCOPY) --input-target binary --output-target elf32-xtensa-le --binary-architecture xtensa --rename-section .data=.rodata.embedded $< $@

