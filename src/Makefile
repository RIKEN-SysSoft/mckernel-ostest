# Makefile COPYRIGHT FUJITSU LIMITED 2016

all::
	@(cd execve_app; make)
	@(cd hello_world; make)
	@(cd large_bss; make)
	@(cd lv07_read-write_with-glibc; make)
	@(cd lv09_syscall_page_fault_forwqarding; make)
	@(cd lv11_copy_on_write; make)
	@(cd lv12_signal; make)
	@(cd lv14_large_pages; make)
	@(cd lv15_continuous_execution; make)
	@(cd show_affinity; make)
	@(cd socket_TP; make)
	@(cd test_drv; make)
	@(cd test_mck; make)
	-mkdir ../bin
	install -p execve_app/execve_app ../bin/
	install -p hello_world/glibc_hello_world ../bin/
	install -p large_bss/large_bss ../bin/
	install -p lv07_read-write_with-glibc/lv07-st ../bin/
	install -p lv07_read-write_with-glibc/lv07-pth ../bin/
	install -p lv09_syscall_page_fault_forwqarding/lv09-pgf ../bin/
	install -p lv11_copy_on_write/lv11 ../bin/
	install -p lv12_signal/lv12-kill ../bin/
	install -p lv12_signal/lv12-kill-single ../bin/
	install -p lv12_signal/lv12-segv ../bin/
	install -p lv14_large_pages/lv14 ../bin/
	install -p lv15_continuous_execution/lv15-kill ../bin/
	install -p lv15_continuous_execution/lv15-manon ../bin/
	install -p lv15_continuous_execution/lv15-mfile ../bin/
	install -p show_affinity/show_affinity ../bin/
	install -p socket_TP/single_node ../bin/
	install -p socket_TP/2node_send ../bin/
	install -p socket_TP/2node_recv ../bin/
	install -p test_drv/test_mck.ko ../bin/
	install -p test_mck/bin/test_mck ../bin/
	install -p coredump_util/autorun.inf ../bin/
	install -p shellscript/test_shell.sh ../bin/

clean::
	@(cd execve_app; make clean)
	@(cd hello_world; make clean)
	@(cd large_bss; make clean)
	@(cd lv07_read-write_with-glibc; make clean)
	@(cd lv09_syscall_page_fault_forwqarding; make clean)
	@(cd lv11_copy_on_write; make clean)
	@(cd lv12_signal; make clean)
	@(cd lv14_large_pages; make clean)
	@(cd lv15_continuous_execution; make clean)
	@(cd show_affinity; make clean)
	@(cd socket_TP; make clean)
	@(cd test_drv; make clean)
	@(cd test_mck; make clean)
