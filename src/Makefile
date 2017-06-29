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
	mkdir ../bin
	cp execve_app/execve_app ../bin/
	cp hello_world/glibc_hello_world ../bin/
	cp large_bss/large_bss ../bin/
	cp lv07_read-write_with-glibc/lv07-st ../bin/
	cp lv07_read-write_with-glibc/lv07-pth ../bin/
	cp lv09_syscall_page_fault_forwqarding/lv09-pgf ../bin/
	cp lv11_copy_on_write/lv11 ../bin/
	cp lv12_signal/lv12-kill ../bin/
	cp lv12_signal/lv12-kill-single ../bin/
	cp lv12_signal/lv12-segv ../bin/
	cp lv14_large_pages/lv14 ../bin/
	cp lv15_continuous_execution/lv15-kill ../bin/
	cp lv15_continuous_execution/lv15-manon ../bin/
	cp lv15_continuous_execution/lv15-mfile ../bin/
	cp show_affinity/show_affinity ../bin/
	cp socket_TP/single_node ../bin/
	cp socket_TP/2node_send ../bin/
	cp socket_TP/2node_recv ../bin/
	cp test_drv/test_mck.ko ../bin/
	cp test_mck/bin/test_mck ../bin/
	cp coredump_util/autorun.inf ../bin/
	cp shellscript/test_shell.sh ../bin/

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
