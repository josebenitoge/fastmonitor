# fastmonitor

## Descripción

**fastmonitor** es una herramienta ligera en C para monitorizar sesiones de usuarios en sistemas Linux. Permite detectar y expulsar conexiones no autorizadas, registrar eventos en logs con detalles como IP, región y hora, y facilita la administración de accesos mediante un archivo de configuración sencillo.

Ideal para administradores que buscan una solución eficaz para controlar accesos SSH y otras sesiones en servidores expuestos a internet.

---

## Características principales

- Configuración sencilla con archivo `.conf`
- Monitorización de sesiones activas y comandos ejecutados
- Expulsión automática de usuarios no autorizados
- Registro de eventos en logs con detalles geográficos (IP, país, región)
- Comandos para iniciar, detener, ver estado y mostrar logs del monitor
- Funciona en segundo plano para un control continuo

---

## Uso

El programa `monitor` soporta los siguientes comandos:

```bash
./monitor start    # Inicia la monitorización en segundo plano
./monitor stop     # Detiene la monitorización
./monitor status   # Muestra el estado actual del monitor y su PID
./monitor log      # Muestra el log con colores para facilitar la lectura

##Ejemplo de archivo de configuracion

#start
polling:10
users:[jb, root, admin]
#end

