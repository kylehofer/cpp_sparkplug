SUPPORTED_COMMANDS := src tests
SUPPORTS_MAKE_ARGS := $(findstring $(firstword $(MAKECMDGOALS)), $(SUPPORTED_COMMANDS))
ifneq "$(SUPPORTS_MAKE_ARGS)" ""
  # use the rest as arguments for the command
  COMMAND_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(COMMAND_ARGS):;@:)
endif

all: src tests

src:
	make -C ./src $(COMMAND_ARGS)

tests:
	make -C ./tests $(COMMAND_ARGS)

docker:
	bash ./buildDocker.sh

clean:
	make -C ./src clean
	make -C ./tests clean

# clean:
# 	make clean -C ./src

.PHONY: all clean src tests