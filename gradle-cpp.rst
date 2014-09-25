====================================
Construir un proyecto C++ con Gradle
====================================

.. role:: math(raw)
   :format: html latex
..

Introducción
============

La construcción, gestión de dependencias y distribución de mis proyectos
es algo que cada vez me preocupa más, me gustaría encontrar una
herramienta que unificara este proceso y fuese independiente del
lenguaje, lo más parecido con lo que he trabajado ha sido
`SCons <http://www.scons.org/>`__,
`Autotools <http://www.gnu.org/software/automake/manual/html_node/Autotools-Introduction.html#Autotools-Introduction>`__,
`Ant <http://ant.apache.org/>`__, `Maven <http://maven.apache.org/>`__ y
últimamente `Gradle <http://www.gradle.org/>`__.

Llevo un tiempo haciendo algunas cosas con
`Gradle <http://www.gradle.org/>`__, pero siempre centrado en proyectos
Java utilizándolo como reemplazo a Maven, porque que es más sencillo de
usar y más rápido. También lo he utilizado `en projectos
Android <http://developer.android.com/sdk/installing/studio-build.html>`__
y he sufrido la pareja `Android Studio +
Gradle <http://developer.android.com/sdk/installing/studio-build.html>`__
en sus primeros matrimonios (porque yo he querido), actualmente está
todo mucho más documentado y funciona muy bien.

Antes de nada hay que decir que la construcción de proyectos C/C++ y
Objective-C con `Gradle <http://www.gradle.org/>`__ se encuentra en fase
de incubación, aunque ya permite hacer algunas tareas avanzadas como:

-  Generación de múltiples artefactos dentro del mismo proyecto, esto es
   varias librerías o ejecutables.

-  Gestionar las dependencias entre estos artefactos (sin versiones).

-  Generar distintos “sabores” de aplicaciones, por ejemplo: podremos
   generar una aplicación “Community” y otra con más características
   habilitadas llamada “Enterprise”.

-  Permite generar binarios para distintas plataformas, esto depende de
   las cadenas de herramientas
   (`Toolchains <http://es.wikipedia.org/wiki/Cadena_de_herramientas>`__)
   que tengamos instaladas en la máquina de compilación.

Como decía todavía tiene limitaciones, aunque están trabajando en ello y
`si consiguen lo que tienen
planeado <http://www.gradleware.com/resources/cpp/>`__\ dejaré
`Autotools <http://www.gnu.org/software/automake/manual/html_node/Autotools-Introduction.html#Autotools-Introduction>`__
(me arrepentiré de haber dicho esto).

Un caso práctico
================

Básicamente he sacado todo el ejemplo de
`aquí <http://www.gradle.org/docs/current/userguide/nativeBinaries.html>`__
y lo he adaptado a un caso en el que hay varias plataformas y quiero
generar dos versiones distintas de mi software “Community” y
“Enterprise”.

La aplicación es un ejecutable y una librería dinámica. El ejecutable
hace uso de esta librería. Ya está, solo quiero mostrar lo que nos
permite hacer `Gradle <http://www.gradle.org/>`__.

También nos permitirá generar una versión para distribuir y otra para
depurar.

Todo el código se encuentra en https://github.com/carlosvin/cpp_gradle.

Estructura del proyecto
-----------------------

Podemos crear la estructura que queramos, pero resulta más fácil seguir
la que espera Gradle, para no tener que especificar donde está el código
fuente. Esta es la estructura del proyecto:

gradle\_cpp
     

    build.gradle
        Fichero donde se configura el proyecto Gradle, el equivalente al
        build.xml de Ant, al Makefile de C/C++ o al pom.xml de Maven.

    src
        Carpeta donde va todo el código fuente

        hello
            Carpeta que contiene el módulo que va a ser la librería
            hello.

            cpp
                Carpeta donde van los fuentes C++.

                Hello.cpp
                     

            headers
                Carpeta donde val los ficheros de cabeceras.

                Hello.h
                     

                Msg.h
                     

        main
            Carpeta que contiene el módulo que será el ejecutable que
            utilice la librería hello.

            cpp
                Carpeta donde van los fuentes C++.

                main.cpp
                     

    build
        Carpeta que crea Gradle automáticamente donde deja todos los
        resultados sus ejecuciones, en ella encontraremos informes de
        resultados de pruebas, binarios compilados, paquetes para
        distribuir, etc.

La aplicación C++
-----------------

Va a consistir en un ejecutable que hará uso de la funcionalidad de la
librería ’hello’.

[H]

::

    #include "Hello.h"
    int main(int argc, char ** argv) 
    {   
        Hello hello ("Pepito");
        hello.sayHello(10);
        return 0; 
    }

Esta librería permite saludar :math:`n` veces a una persona especificada
en su constructor.

[H]

::

    class Hello  
    {
        private:
            const char * who;
        public:
            Hello(const char * who);
            void sayHello(unsigned n = 1);
    };

Construyendo con Gradle
-----------------------

Caso básico
~~~~~~~~~~~

Lo único que necesitamos para construir nuestra aplicación con Gradle
es: tener Gradle [1]_ y el fichero build.gradle.

[H]

::

    apply plugin: 'cpp'

    libraries {     
        hello {} 
    }
    executables {     
        main {
            binaries.all {
                lib libraries.hello.shared         
            }
        }
    }

Con este fichero tan simple, conseguiremos compilar e instalar nuestra
aplicación, en modo Debug para la plataforma donde estamos ejecutando
gradle, en mi caso es Linux X64.

Si ejecutamos desde la raíz de nuestro proyecto gradle task, podremos
ver todas las tareas que podemos hacer.

En nuestro caso, solo queremos nuestra aplicación compilada y lista para
funcionar, así que ejecutaremos: gradle installMainExecutable.

Una vez que ha terminado, podemos ejecutar el programa llamando al
script  [2]_.

[H]

::

    $ build/install/mainExecutable/main.bat
    1. Hello Mr. Pepito (Community) 
    2. Hello Mr. Pepito (Community) 
    3. Hello Mr. Pepito (Community) 
    4. Hello Mr. Pepito (Community) 
    5. Hello Mr. Pepito (Community) 
    6. Hello Mr. Pepito (Community) 
    7. Hello Mr. Pepito (Community) 
    8. Hello Mr. Pepito (Community) 
    9. Hello Mr. Pepito (Community) 
    10. Hello Mr. Pepito (Community) 

Distintos “sabores”
~~~~~~~~~~~~~~~~~~~

Con unas pocas líneas más, podemos generar distintas versiones de la
misma aplicación, en nuestro ejemplo vamos a generar una versión
“Community” y otra “Enterprise”.

[H]

::

    apply plugin: 'cpp'
    model {
        flavors {
            community
            enterprise
        }
    }
    libraries {
        hello {
            binaries.all {             
                if (flavor == flavors.enterprise) {                         cppCompiler.define "ENTERPRISE"
                }
            }
        }
    }
    executables {
        main {
            binaries.all {
                lib libraries.hello.shared
            }
        }
    }

Además tenemos que preparar nuestra aplicación para utilizar estos
parámetros de compilación.

[H]

::

    #ifdef ENTERPRISE
    static const char * EDITION = "Enterprise";

    #else 
    static const char * EDITION = "Community";

    #endif

encuentra en Todo el proyecto se

De esta forma se utiliza una cadena u otra en función del “sabor” con
que compilemos.

Si ahora ejecutamos gradle clean task en la raíz de nuestro proyecto,
veremos que tenemos más tareas disponibles, antes teníamos
installMainExecutable y ahora ha sido reemplazada por
installCommunityMainExecutable y installEnterpriseMainExecutable.

Si ejecutamos estas dos tareas , tendremos nuestra aplicación instalada
en los dos sabores.

[H]

::

    $gradle installEnterpriseMainExecutable installCommunityMainExecutable

    :compileEnterpriseHelloSharedLibraryHelloCpp 
    :linkEnterpriseHelloSharedLibrary 
    :enterpriseHelloSharedLibrary 
    :compileEnterpriseMainExecutableMainCpp 
    :linkEnterpriseMainExecutable 
    :enterpriseMainExecutable 
    :installEnterpriseMainExecutable 
    :compileCommunityHelloSharedLibraryHelloCpp 
    :linkCommunityHelloSharedLibrary 
    :communityHelloSharedLibrary 
    :compileCommunityMainExecutableMainCpp 
    :linkCommunityMainExecutable 
    :communityMainExecutable 
    :installCommunityMainExecutable

    BUILD SUCCESSFUL
    Total time: 9.414 secs 

Ahora podemos ejecutar nuestra aplicación en los dos sabores:

[H]

::

    $ build/install/mainExecutable/community/main.bat
    1.      Hello Mr. Pepito        (Community)
    2.      Hello Mr. Pepito        (Community) 
    3.      Hello Mr. Pepito        (Community) 
    4.      Hello Mr. Pepito        (Community) 
    5.      Hello Mr. Pepito        (Community) 
    6.      Hello Mr. Pepito        (Community) 
    7.      Hello Mr. Pepito        (Community) 
    8.      Hello Mr. Pepito        (Community) 
    9.      Hello Mr. Pepito        (Community) 
    10.     Hello Mr. Pepito        (Community)

[H]

::

    $ build/install/mainExecutable/enterprise/main.bat 
    1.      Hello Mr. Pepito        (Enterprise) 
    2.      Hello Mr. Pepito        (Enterprise) 
    3.      Hello Mr. Pepito        (Enterprise) 
    4.      Hello Mr. Pepito        (Enterprise) 
    5.      Hello Mr. Pepito        (Enterprise) 
    6.      Hello Mr. Pepito        (Enterprise) 
    7.      Hello Mr. Pepito        (Enterprise) 
    8.      Hello Mr. Pepito        (Enterprise) 
    9.      Hello Mr. Pepito        (Enterprise) 
    10.     Hello Mr. Pepito        (Enterprise)

Release o Debug
~~~~~~~~~~~~~~~

Por defecto Gradle compila nuestra aplicación en modo Debug, pero
podemos añadir el modo Release para que active algunas
optimizaciones [3]_.

[H]

::

    apply plugin: 'cpp'
    model {
        buildTypes {
            debug         
            release
        }
    // ... the rest of file below doesn't change 

Si ahora ejecutamos gradle clean task veremos que tenemos más tareas, se
habrán desdoblado las que teníamos, por ejemplo
installCommunityMainExecutable se habrá desdoblado en
installDebugCommunityMainExecutable y
installReleaseCommunityMainExecutable.

Multi-plataforma
~~~~~~~~~~~~~~~~

También tenemos las posibilidad de utilizar las características de
compilación cruzada que nos ofrecen los compiladores y generar
componentes nativos para otras aplicaciones. El proceso es el mismo,
simplemente tenemos que dar te alta las aplicaciones con las que vamos a
trabajar.

Esto solo funcionará si en nuestro sistema tenemos instalada la cadena
de herramientas
(`Toolchains <http://es.wikipedia.org/wiki/Cadena_de_herramientas>`__)
necesaria, es decir, si en un sistema de 64 bits queremos compilar para
32 bits, tendremos que tener instaladas las librerías necesarias en en
32 bits.

[H]

::

    apply plugin: 'cpp'
    model {
        buildTypes {
            debug
            release
        }
             platforms {
            x86 {
                architecture "x86"
            }
            x64 {
                architecture "x86_64"
            }
            itanium {
                architecture "ia-64"
            }
        } 
        flavors {
            community
            enterprise
        }
    }
    libraries {
        hello {
            binaries.all {
                if (flavor == flavors.enterprise) {
                    cppCompiler.define "ENTERPRISE"
                }
            }
        }
    }
    executables {
        main {
            binaries.all {
                lib libraries.hello.shared
            }
        }
    }

Ejecutando gradle clean task podremos generar distintas versiones de
nuestra aplicación en distintos sabores, para distintas aplicaciones en
Debug o Release.

Conclusiones
============

Con una configuración mínima, tenemos muchas posibilidades de
construcción de aplicaciones nativas multi-plataforma.

Tiene un futuro prometedor, veremos como termina.

Podemos utilizar otras características de Gradle y aplicarlas a nuestros
proyectos C++, como análisis estáticos de código, generación de informes
de prueba, fácil incorporación a sistemas de integración continua.

Gradle para C++ es una característica que actualmente está en
desarrollo, por lo que:

-  No debemos utilizar en entornos reales de desarrollo, puede acarrear
   muchos dolores de cabeza.

-  La forma de definir el fichero build.gradle puede cambiar.

Todo el ejemplo se encuentra en https://github.com/carlosvin/cpp_gradle.

.. [1]
   Realmente no es necesario tener instalado Gradle, si utilizamos el
   wrapper, pero esto no lo vamos a tratar hoy,\ `si queréis más
   información <http://www.gradle.org/docs/current/userguide/nativeBinaries.html>`__.

.. [2]
   .bat en Windows y .sh en Linux

.. [3]
   También podemos definir el tipo de optimizaciones que vamos a
   utilizar.
