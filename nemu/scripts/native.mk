include $(NEMU_HOME)/scripts/git.mk
include $(NEMU_HOME)/scripts/build.mk

include $(NEMU_HOME)/tools/difftest.mk

compile_git:
	$(call git_commit, "compile")
$(BINARY): compile_git

# Some convenient rules

override ARGS ?= --log=$(BUILD_DIR)/nemu-log.txt
override ARGS += $(ARGS_DIFF)

# Command to execute NEMU
IMG ?=
NEMU_EXEC := $(BINARY) $(ARGS) $(IMG)

run-env: $(BINARY) $(DIFF_REF_SO)

run: run-env
	$(call git_commit, "run")
	$(NEMU_EXEC)

gdb: run-env
	$(call git_commit, "gdb")
	gdb -s $(BINARY) --args $(NEMU_EXEC)

vscode: run-env
	$(call git_commit, "gdb")

count:
	lines_count_pa1=`find . | grep '\.c$\|\.h$' | xargs wc -l | grep 'total' | awk '{print substr($1, 1)}'`
	git checkout pa0 >/dev/null 2>/dev/null
	lines_count_pa0=`find . | grep '\.c$\|\.h$' | xargs wc -l | grep 'total' | awk '{print substr($1, 1)}'`
	git checkout pa1 >/dev/null 2>/dev/null
	expr="$lines_count_pa1 - $lines_count_pa0"
	echo New lines: `expr $expr`

clean-tools = $(dir $(shell find ./tools -name "Makefile"))
$(clean-tools):
	-@$(MAKE) -s -C $@ clean
clean-tools: $(clean-tools)
clean-all: clean distclean clean-tools

.PHONY: run gdb vscode count run-env clean-tools clean-all $(clean-tools)
