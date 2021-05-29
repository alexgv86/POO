/*
 * espagueti.c
 * 
 * Copyright 2020 grchere <grchere@yahoo.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * Ejemplo de Codigo Espagueti, romerizado
 * Objetivo del programa: hacer altas, bajas y modificaciones
 * en archivo de acceso directo
 * donde codigo es un numero entero de 1 a 100
 * en donde la posicion fisica 0 de registro corresponde al codigo 1
 * la posicion 1 al codigo 2 y asi sucesivamente
 * el byte de estado vale 0 cuando esta libre el registro, 1 cuando
 * esta ocupado y 2 cuando esta borrado
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct rCli {
	char estado;
	int codigo;
	char nombre[100];
} rCli;

int main(int argc, char **argv)
{
	FILE *f = fopen("clientes.dat","r+b");
	if ( f == NULL ) {
		// posiblemente el archivo no existe, lo creo con 100 registros
		f = fopen("clientes.dat","wb");
		rCli r;
		memset(&r,0,sizeof(rCli));
		int i;
		for(i=0;i<100;i++) fwrite(&r,sizeof(rCli),1,f);
		fclose(f);
		// vuelvo a abrir el archivo
		f = fopen("clientes.dat","r+b");
	}
	int continuar = 1;
	do {
		char scodigo[10];
		int ncodigo;
		rCli r;
		printf("Ingrese Codigo (-1 para salir):");
		fgets(scodigo,10,stdin);
		ncodigo = atoi(scodigo);
		if ( ncodigo == -1 ) {
			continuar = 0;
		} else {
			// pretende continuar, busco registro en archivo
			// valido codigo
			if ( ncodigo >= 1 && ncodigo <= 100 ) {
				// me posiciono en el archivo justo antes de que comience el registro buscado
				fseek(f,(ncodigo - 1) * sizeof(rCli),SEEK_SET);
				// leo registro
				fread(&r,sizeof(rCli),1,f);
				switch(r.estado) {
					case 0:
						printf("Registro Vacio!, puede darlo de alta\n");
						break;
					case 1:
						printf("Registro Ocupado!, puede modificarlo o borrarlo\n");
						// muestro registro a modificar
						printf("Nombre: %s\n",r.nombre);
						printf("Â¿Desea borrarlo? (s/n):");
						char rta[3];
						fgets(rta,3,stdin);
						if ( strncmp(rta,"s",1) == 0 ) {
							// borro registro
							r.estado = 2;
							fseek(f,(ncodigo - 1) * sizeof(rCli),SEEK_SET);
							fwrite(&r,sizeof(rCli),1,f);
							printf("Registro borrado!\n");
							r.estado = 3; // para evitar el ingreso de datos
						}
						break;
					case 2:
						printf("Registro Borrado!, puede darlo de alta\n");
						break;
				}
				if ( r.estado != 3 ) {
					// ingreso datos
					r.codigo = ncodigo;
					r.estado = 1;
					printf("Nombre:");
					fgets(r.nombre,100,stdin);
					fseek(f,(ncodigo - 1) * sizeof(rCli),SEEK_SET);
					fwrite(&r,sizeof(rCli),1,f);
					printf("Registro actualizado Ok!\n");
				}
			} else {
				printf("Error, codigo debe ser un valor entre 1 y 100\n");
			}
		}
	} while(continuar);
	fclose(f);
	return 0;
}
