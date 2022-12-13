SUPPORTED_COMMANDS := src tests
SUPPORTS_MAKE_ARGS := $(findstring $(firstword $(MAKECMDGOALS)), $(SUPPORTED_COMMANDS))
ifneq "$(SUPPORTS_MAKE_ARGS)" ""
  # use the rest as arguments for the command
  COMMAND_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(COMMAND_ARGS):;@:)
endif

all: src

src:
	make -C ./src $(COMMAND_ARGS)

tests:
	make -C ./tests $(COMMAND_ARGS)

docker:
	bash ./buildDocker.sh

copy:
	 sshpass -p $(PASSWORD) rsync -rav -e ssh --exclude='build/' --exclude='.git/' --exclude='temp/' ./ $(USER)@$(HOST):$(TARGET)

# clean:
# 	make clean -C ./src

.PHONY: all clean src tests