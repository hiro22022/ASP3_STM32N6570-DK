# ASP3 kernel source lists (ALLFUNC mode, -DALLFUNC)

set(ASP3_KERNEL_FCSRCS
    kernel/startup.c
    kernel/task.c
    kernel/taskhook.c
    kernel/wait.c
    kernel/time_event.c
    kernel/task_manage.c
    kernel/task_refer.c
    kernel/task_sync.c
    kernel/task_term.c
    kernel/semaphore.c
    kernel/eventflag.c
    kernel/dataqueue.c
    kernel/pridataq.c
    kernel/mutex.c
    kernel/mempfix.c
    kernel/time_manage.c
    kernel/cyclic.c
    kernel/alarm.c
    kernel/sys_manage.c
    kernel/interrupt.c
    kernel/exception.c
)

set(ASP3_KERNEL_EXTRA_CSRCS
    arch/arm_m_gcc/common/core_kernel_impl.c
)

set(ASP3_KERNEL_ASM_SRCS
    arch/arm_m_gcc/common/core_support.S
)

function(asp3_add_kernel_library target_name)
    add_library(${target_name} STATIC)

    foreach(_src ${ASP3_KERNEL_FCSRCS})
        target_sources(${target_name} PRIVATE "${ASP3_SRCDIR}/${_src}")
    endforeach()
    foreach(_src ${ASP3_KERNEL_EXTRA_CSRCS})
        target_sources(${target_name} PRIVATE "${ASP3_SRCDIR}/${_src}")
    endforeach()
    foreach(_src ${ASP3_KERNEL_ASM_SRCS})
        target_sources(${target_name} PRIVATE "${ASP3_SRCDIR}/${_src}")
    endforeach()

    target_sources(${target_name} PRIVATE ${ASP3_TARGET_KERNEL_SOURCES})

    target_compile_definitions(${target_name} PRIVATE ALLFUNC)
    target_include_directories(${target_name} PRIVATE
        "${ASP3_SRCDIR}/include"
        "${ASP3_SRCDIR}/kernel"
        ${ASP3_TARGET_INCLUDE_DIRS}
        ${ASP3_ARCH_INCLUDE_DIRS}
    )
    target_compile_options(${target_name} PRIVATE ${ASP3_TARGET_COMPILE_OPTIONS})
    target_compile_definitions(${target_name} PRIVATE ${ASP3_TARGET_COMPILE_DEFINITIONS})
endfunction()
