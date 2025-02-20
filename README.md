Los branches en Git deben utilizarse para separar características, correcciones de errores o pruebas experimentales. Algunas ideas para organizar tus branches:

    main o master → Contiene la versión estable del firmware.
    develop → Rama de desarrollo donde se integran cambios antes de ser probados en producción.
    feature/<nombre> → Para nuevas características. Por ejemplo:
        feature/pwm-control
        feature/sleep-mode
        feature/adc-sensor
    bugfix/<nombre> → Para corregir errores. Por ejemplo:
        bugfix/pwm-inversion
        bugfix/timer-overflow
    experiment/<nombre> → Para pruebas experimentales sin afectar la rama principal.
        experiment/new-bootloader
        experiment/faster-adc
    hotfix/<nombre> → Para correcciones urgentes en producción.

Cada característica nueva o corrección de errores debería estar en su propia rama feature/ o bugfix/ y fusionarse a develop antes de ser incluida en main.

dame un ejemplo
