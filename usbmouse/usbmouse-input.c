struct input_dev {
    name
    phys
    id
    dev
    evbit
    keybit
    relbit
    open
    close
}

input_allocate_device()
input_free_device()
input_register_device()
input_unregister_device()
input_get_drvdata()
input_set_drvdata()
input_report_key()
input_report_rel()
input_sync()
