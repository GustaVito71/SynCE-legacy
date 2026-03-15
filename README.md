# SynCE Legacy

Meta-repositorio para el port y modernización de SynCE en Debian 12 Bookworm.
Reemplaza Pyrex por Cython 3 y migra de Python 2 a Python 3, manteniendo
compatibilidad con dispositivos Windows CE / Windows Mobile en distribuciones
Linux actuales.

## Estructura del repositorio

Cada paquete es un submódulo Git independiente con dos ramas:

| Submódulo | Descripción | `main` | `bookworm` |
|-----------|-------------|--------|------------|
| `librtfcomp` | Biblioteca para archivos RTF comprimidos | Python 2 original | Port Bookworm |
| `libmimedir` | Parser RFC 2425 (vCard/vCal) | Python 2 original | Port Bookworm |
| `synce-core` | Biblioteca principal + dccm + herramientas | Python 2 original | Port Bookworm |
| `librra` | Remote Replication Agent | Python 2 original | Port Bookworm |
| `synce-sync-engine` | Motor de sincronización | Python 2 original | Port Bookworm |

> El tag `python2-sources` preserva el estado original Python 2 de todos los paquetes.

---

## Guía de construcción e instalación en Debian 12 Bookworm

### Cadena de dependencias

Los paquetes deben construirse e instalarse en este orden:

```
librtfcomp-1.3          (sin dependencias SynCE)
libmimedir-0.5.1        (sin dependencias SynCE)
synce-core-0.17         (sin dependencias SynCE)  → provee libsynce0-dev, python3-rapi2
librra-0.17             (depende de synce-core)   → provee python3-rra
synce-sync-engine-0.16  (depende de todos los anteriores)
```

---

### 1. Preparar el sistema

```bash
sudo apt-get update && sudo apt-get upgrade -y

sudo apt-get install -y \
    git \
    build-essential \
    debhelper \
    devscripts \
    quilt \
    fakeroot \
    pkg-config
```

---

### 2. Obtener los fuentes

```bash
git clone --recurse-submodules \
    https://github.com/GustaVito71/SynCE-legacy.git
cd SynCE-legacy
git submodule foreach git checkout bookworm
```

---

### 3. Construir e instalar cada paquete

---

#### 3.1 librtfcomp

```bash
sudo apt-get install -y python3-all-dev dh-python python3 cython3

cd librtfcomp
rm -rf .pc/
dpkg-buildpackage -b -uc -us
cd ..

sudo dpkg -i librtfcomp0_1.3-0~bookworm1_amd64.deb \
             librtfcomp-dev_1.3-0~bookworm1_amd64.deb \
             python3-rtfcomp_1.3-0~bookworm1_amd64.deb
```

---

#### 3.2 libmimedir

```bash
sudo apt-get install -y dh-autoreconf bison flex libtool libtool-bin

cd libmimedir
rm -rf .pc/
dpkg-buildpackage -b -uc -us
cd ..

sudo dpkg -i libmimedir0_0.5.1-0~bookworm1_amd64.deb \
             libmimedir-dev_0.5.1-0~bookworm1_amd64.deb
```

---

#### 3.3 synce-core

```bash
sudo apt-get install -y \
    libdbus-1-dev libglib2.0-dev ppp net-tools \
    isc-dhcp-client libgudev-1.0-dev udev cython3 python3-dev

cd synce-core
rm -rf .pc/
dpkg-buildpackage -b -uc -us
cd ..

sudo dpkg -i libsynce0_0.17-0~bookworm8_amd64.deb \
             libsynce0-dev_0.17-0~bookworm8_amd64.deb \
             python3-rapi2_0.17-0~bookworm8_amd64.deb \
             synce-tools_0.17-0~bookworm8_amd64.deb \
             synce-dccm_0.17-0~bookworm8_amd64.deb
```

---

#### 3.4 librra

```bash
sudo apt-get install -y libglib2.0-dev cython3 python3-dev

cd librra
rm -rf .pc/
dpkg-buildpackage -b -uc -us
cd ..

sudo dpkg -i librra0_0.17-0~bookworm2_amd64.deb \
             librra-dev_0.17-0~bookworm2_amd64.deb \
             librra-tools_0.17-0~bookworm2_amd64.deb \
             python3-rra_0.17-0~bookworm2_amd64.deb
```

---

#### 3.5 synce-sync-engine

```bash
sudo apt-get install -y \
    python3-all python3-setuptools \
    python3-lxml python3-dbus

cd synce-sync-engine
rm -rf .pc/
dpkg-buildpackage -b -uc -us
cd ..

sudo dpkg -i synce-sync-engine_0.16-0~bookworm2_all.deb
```

---

### 4. Verificar la instalación

```bash
# Bindings Python
python3 -c "import pyrtfcomp; print('pyrtfcomp OK')"
python3 -c "import pyrapi2;  print('pyrapi2 OK')"
python3 -c "import pyrra;    print('pyrra OK')"

# Archivos de sistema
ls /usr/share/dbus-1/system-services/org.synce.dccm.service
ls /lib/udev/rules.d/85-synce.rules

# Conectar el dispositivo y verificar
ss -tnp | grep 5679        # debe mostrar ESTAB con la IP del dispositivo
synce-pstatus              # muestra versión WinCE, batería, memoria
```

> **Nota:** `synce-dccm` usa activación D-Bus, no systemd. No existe `synce-dccm.service` en systemd.
> El mensaje `dbus.proxies ERROR: Introspect error` en el journal es no fatal — el stack funciona correctamente.

---

### 5. Resolución de problemas

| Síntoma | Causa probable | Solución |
|---------|----------------|----------|
| `dpkg -i` falla con dependencias | Paquetes instalados fuera de orden | Respetar el orden de la sección 3 |
| `quilt push` falla al compilar | Patches ya aplicados sin estado `.pc/` | Ejecutar el bloque de inicialización quilt |
| `EADDRINUSE` al reconectar | Socket stale de dccm | Corregido en bookworm8 — reinstalar |
| `import pyrapi2` falla | `python3-rapi2` no instalado | `sudo dpkg -i python3-rapi2_*.deb` |
