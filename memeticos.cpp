#include "memeticos.h"
#include <iostream>


/***************Funciones para las tablas*********************/

void loadTable(){
    string linea;
    bool leyendo_datos=false;

    int i=0,j=0,aux=0,ind=0;
    float *maximos, *minimos;
    ifstream fich_tabla(path,fstream::in);
    if(!fich_tabla.good()){
        cout<<"Error al abrir el fichero: "<<path<<endl;
        exit(-1);
    }
    maximos = new float[n_caracteristicas];
    minimos = new float[n_caracteristicas];
    bool spambase = false;
    int num_datos = n_cadenas;
    if(string(path).compare("./tablas/spambase.arff")==0){
        spambase = true;
        num_datos = 4601;
    }

    while(!fich_tabla.eof() && i<num_datos){
        if(!leyendo_datos){
            //Pasamos de las lineas hasta llegar a los datos
            getline(fich_tabla,linea);
            if(linea.compare(0,5,"@data")==0 || linea.compare(0,5,"@DATA")==0){ //Paramos de descartar lineasporque hemos llegado a los datos
                leyendo_datos=true;
            }
        }else{
            if(spambase && i>n_cadenas){
                if(aux == 0){
                    getline(fich_tabla,linea);
                    j=0;
                    i++;
                    aux = Randint(0,1);
                }else{
                    fich_tabla >> datos[ind][j];
                    if(i==0){
                        maximos[j] = datos[ind][j];
                        minimos[j] = datos[ind][j];
                    }else{
                        if(maximos[j] < datos[ind][j])
                            maximos[j] = datos[ind][j];
                        if(minimos[j] > datos[ind][j])
                            minimos[j] = datos[ind][j];
                    }
                    fich_tabla.ignore(1,',');
                    j++;
                }
            }else{
                //Ahora que hemos encontrado los datos los leemos y guardamos en la matriz datos
                fich_tabla >> datos[i][j];
                if(i==0){
                    maximos[j] = datos[i][j];
                    minimos[j] = datos[i][j];
                }else{
                    if(maximos[j] < datos[i][j])
                        maximos[j] = datos[i][j];
                    if(minimos[j] > datos[i][j])
                        minimos[j] = datos[i][j];
                }
                fich_tabla.ignore(1,',');
                j++;
            }
            //Una vez que j>tam_cadena significa que ya hemos leido los datos de la cadena, el último valor es la clase así que las clases las almacenamos en el array clases
            if (j==n_caracteristicas){
                if(string(path).compare("./tablas/sonar.arff")==0){
                    getline(fich_tabla, linea);
                    if (linea.compare("Rock")==0)
                        nclases[i]=0;
                    else
                        nclases[i]=1;
                }else{
                    if(i>n_cadenas && aux==1)
                        fich_tabla >> nclases[ind];
                    else
                        fich_tabla >> nclases[i];
                }
                j=0;
                i++;
                aux = Randint(0,1);
                ind = Randint(0,n_caracteristicas-1);
            }
        }
    }
    fich_tabla.close();

    for(int i=0; i<n_cadenas; i++)
        for(int j=0; j<n_caracteristicas; j++){
            if ((maximos[j]-minimos[j]) != 0)
                datos[i][j] = (datos[i][j] - minimos[j]) / (maximos[j]-minimos[j]);
        }
}



void shuffle(){
    int ind =0;
    int clase = 0;
    float auxx;


    for(int i =0; i< n_cadenas; i++){

        ind = Randint(0,n_cadenas-1);
        clase = nclases[i];
        nclases[i] = nclases[ind];
        nclases[ind] = clase;
        for(int j = 0; j<n_caracteristicas; j++){

            auxx = datos[i][j];
            datos[i][j] = datos[ind][j];
            datos[ind][j] = auxx;
        }
    }
}





/*************Funciones para calcular valoracion**************/


float calcularTasaRed(){

    int n =0;
    for(int i=0; i<n_caracteristicas; i++)
        if(sol.s[i])
            n++;
    return (float)(n_caracteristicas - n) * 100.0/n_caracteristicas;


}



int pmax(float* d_v, int n){

    int max = 0;

    for(unsigned i=1; i<n; i++){

        if(d_v[max]<d_v[i])
            max = i;

    }
    return max;

}




float distancia(float * d1, float *d2, int n, s_caracteristicas aux){

    float d = 0.0;

    for(int i=0; i<n; i++){

        if(aux.s[i])
            d = d + ((d2[i]-d1[i]) * (d2[i]-d1[i]));

    }

    return sqrt(d);
}



s_caracteristicas generarSolRandom(){
    sol.tam = n_caracteristicas;
    sol.s = new bool[n_caracteristicas];
    for(int i=0; i<n_caracteristicas; i++){
        sol.s[i] = Randint(0,1);
    }
    sol.tasa_clas = 0;
    sol.tasa_red = calcularTasaRed();
    return sol;
}





float distancia(float * d1, float *d2, int n){

    float d = 0.0;

    for(int i=0; i<n; i++){

        if(sol.s[i])
            d = d + ((d2[i]-d1[i]) * (d2[i]-d1[i]));

    }
    return sqrt(d);
}


/*******************************KNN***********************************/



int knn(int ini, int n, int k, int e){
    int clase= 0;
    float vmax=0;
    int p_vmax=0;
    int vecinos[k];
    float d_vecinos[k], dist;
    int i,j,solucion=0;

    //Este for coge los 3 primeros vecinos como los mas cercanos y guardamos la posicion y el valor de la distancia del vecino que tiene la distancia maxima de esos 3
    for(i=ini, j=0; j<k; i++, j++){

        if(i>=n_cadenas)
            i=0;

        if(i==e)
            i = (i+1)%n_cadenas;

        vecinos[j] = i;
        d_vecinos[j] = distancia(datos[i], datos[e], n_caracteristicas);

        if (vmax<d_vecinos[j]){

            vmax = d_vecinos[j];
            p_vmax = j;
        }
    }

    //Luego en este for recorro los vecinos que quedan por mirar (osea todos menos los 3 que hemos cogido de primeras) y si alguno mejora al que tiene distancia maxima de los tres anteriores lo sustituye
    for(j=k; j<n; j++, i++){

        if(i>=n_cadenas)
            i=0;

        if(i==e)
            i = (i+1)%n_cadenas;

        dist = distancia(datos[i], datos[e], n_caracteristicas);

        if(vmax>dist){

            vecinos[p_vmax] = i;
            d_vecinos[p_vmax] = dist;
            p_vmax = pmax(d_vecinos, k);
            vmax = d_vecinos[p_vmax];
        }
    }

    for (int i=0; i<k; i++){

        clase = clase + nclases[vecinos[i]];
    }

     if(clase > k*0.5)
        solucion = 1;


        return solucion;
}




int knn(int ini, int n, int k, int e, s_caracteristicas aux){

    int clase= 0, p_vmax=0;
    float vmax=0;
    int vecinos[k];
    float d_vecinos[k], dist;
    int i,j,solucion=0;



    for(i=ini, j=0; j<k; i++, j++){

        if(i>=n_cadenas)
            i=0;

        if(i==e)
            i = (i+1)%n_cadenas;

        vecinos[j] = i;
        d_vecinos[j] = distancia(datos[i], datos[e], n_caracteristicas, aux);

        if (vmax<d_vecinos[j]){

            vmax = d_vecinos[j];
            p_vmax = j;

        }
    }

    for(j=k; j<n; j++, i++){

        if(i>=n_cadenas)
            i=0;

        if(i==e)
            i = (i+1)%n_cadenas;

        dist = distancia(datos[i], datos[e], n_caracteristicas, aux);

        if(vmax>dist){

            vecinos[p_vmax] = i;
            d_vecinos[p_vmax] = dist;
            p_vmax = pmax(d_vecinos, k);
            vmax = d_vecinos[p_vmax];

        }
    }

    for (int v=0; v<k; v++){

        clase = clase + nclases[vecinos[v]];
    }

    if(clase > k*0.5)
        solucion = 1;


        return solucion;
}



/*****************************Funciones de coste****************************************/

s_caracteristicas sol_copy(s_caracteristicas aux){

    s_caracteristicas copy_sol;

    copy_sol.s = new bool [n_caracteristicas];

    for(int i=0; i<n_caracteristicas; i++){

        copy_sol.s[i] = aux.s[i];

    }

    copy_sol.tasa_red = aux.tasa_red;
    copy_sol.tasa_clas = aux.tasa_clas;

    return copy_sol;
}



float coste(int ini, int tam_conjunto, int k, s_caracteristicas aux){

    float acierto=0.0;
    float tam = tam_conjunto;
    int clase_propuesta;

    for (int cont=0,i=ini; cont<tam_conjunto; cont++,i++){
        if(i==n_cadenas)
            i=0;
        clase_propuesta = knn(ini,tam_conjunto,k,i, aux);

        if(clase_propuesta==nclases[i])
            acierto=acierto+1.0;
    }
    return (acierto/tam)*100.0;
}



float coste(int ini, int tam_conjunto, int k){

    float acierto=0.0;
    float tam = tam_conjunto;
    int clase_propuesta;

    for (int cont=0,i=ini; cont<tam_conjunto; cont++,i++){
        if(i==n_cadenas)
            i=0;
        clase_propuesta = knn(ini,tam_conjunto,k,i);
        if(clase_propuesta==nclases[i])
            acierto=acierto+1.0;
    }
    return (acierto/tam)*100.0;
}






/***********************************GREEDY****************************/

s_caracteristicas greedySFS(int ini, int tam , int k){

    sol.tam = n_caracteristicas;
    sol.s = new bool[n_caracteristicas];
    sol.tasa_clas = 0.0;
    float tasa_clas=0.0;
    int ind = 0;


    for(int i=0; i<n_caracteristicas; i++){

        sol.s[i] = false;
    }




    for(int i=0; i<n_caracteristicas; i++){

        for(int j=0; j<n_caracteristicas; j++){

            if(!sol.s[j]){

                sol.s[j] = true;
                sol.tasa_clas = coste(ini, tam, k);

                if(sol.tasa_clas>tasa_clas){

                    ind = j;
                    tasa_clas = sol.tasa_clas;
                }
                sol.s[j] =false;
            }
        }
        if(sol.s[ind] == true)
            i = n_caracteristicas;
        else{
            sol.s[ind] = true;
        }
    }
    sol.tasa_clas = coste(ini, tam, k);
    sol.tasa_red = calcularTasaRed();

    return sol;
}





/*******************************************************************/
/**********************EMPIEZAEL ALGORITMO MEMETICO*****************/
/*******************************************************************/







/*********************************LOCAL SEARCH*******************/

s_caracteristicas LocalSearch(int ini,int tam ,int k, s_caracteristicas aux, int&iteracion){

    bool mejora=false;
    int ind = Randint(0,n_caracteristicas-1), cont = 0;
    float humbral = coste(ini, tam, k, aux);

    for(unsigned int i=0; i<n_caracteristicas && i<200 && !mejora; i++){

        aux.s[i]=!aux.s[i];
        aux.tasa_clas = coste(ini, tam, k, aux);
        iteracion++;

        if(aux.tasa_clas > humbral){
            aux.tasa_red = calcularTasaRed();
            mejora = true;
        }
    }
        return aux;

}




/**************************FUNCIONES PARA EL ALGORITMO Memetico********/

//Operador de cruce desde dos puntos al azar  que determinan 3 subcadenas

void cruce(s_caracteristicas padre1, s_caracteristicas padre2, s_caracteristicas hijo1, s_caracteristicas hijo2){

    int rand1 = Randint(0, n_caracteristicas-2);
    int rand2 = Randint(rand1, n_caracteristicas-1);

    for(int i=0; i<n_caracteristicas; i++){

        if(i<rand1){
            hijo1.s[i] = padre1.s[i];
            hijo2.s[i] = padre2.s[i];
        }else
            if(i>=rand1 && i<rand2){
                hijo1.s[i] = padre2.s[i];
                hijo2.s[i] = padre1.s[i];
            }else{

                hijo1.s[i] = padre1.s[i];
                hijo2.s[i] = padre2.s[i];

            }


    }

}


//Operador de seleccion de padres

s_caracteristicas seleccionPadres(s_caracteristicas padre1,s_caracteristicas padre2){

    if(padre1.tasa_clas > padre2.tasa_clas)
        return sol_copy(padre1);
    else
        return sol_copy(padre2);


}

//Una vez seleccionado el gen a mutar, lo muta utilizando el operador flip

void mutacion(s_caracteristicas s, int gen){

    s.s[gen]=!s.s[gen];

}

/***********************************************************************/

//AGG
/*
s_caracteristicas AGG (int inicio, int tam, int k, int n_interaciones){
    cout << "empieza el algoritmo Memetico" << endl;
    int n_cruces = 0.7 * (10 * 0.5);
    int n_padres = num_cruces * 2;
    int n_mutaciones = (10 * n_caracteristicas);
    int p_mejor=0;
    int p_peor=0;
    bool seleccionado = false;

    s_caracteristicas poblacion[10];
    s_caracteristicas padres[10];
    s_caracteristicas mejor;
    mejor.tasa_clas = 0.0:
    float peor = 100.0;

    //Inicializamos la poblacion actual

    for (int i = 0; i < 10 ; ++i)
    {
        poblacion[i] = generarSolRandom();
        poblacion[i].tasa_clas = coste(ini, tam, k);

    }

    int iteracion=10;

    while(iteracion<n_interaciones){



        //Seleccion del mejor individuo de la poblacion

     for (int i = 0; i < 10; ++i)
     {
        if(poblacion[i].tasa_clas>mejor.tasa_clas){
            mejor = sol_copy(poblacion[i]);
            p_mejor = i;
        }

     }

         //seleccion de los padres

      for (int i = 0; i < 10; ++i)
     {
         r1 = Randint(0,9);
         r2 = Randint(0,9);

          padres[i] = seleccionPadres(poblacion[r1],poblacion[r2]);

           if(r1==p_mejor || r2==p_mejor){
                seleccionado = true;
        }

     }



      //Cruce de padres

       for (int i = 0; i < n_padres; i+=2)
       {
           cruce(padres[i],padres[i+1],poblacion[i],poblacion[i+1]);
           poblacion[i].tasa_clas = coste(ini,tam,k,poblacion[i]);
           poblacion[i+1].tasa_clas = coste(ini,tam,k,poblacion[i+1]);
           iteracion+=2;
       }

       //Rellenamos la poblacion vacia con los padres

      for (int i = n_padres; i < 10; ++i){

          poblacion[i] = padres[i];
      }

        //Realizamos las mutaciones

      for (int i = 0; i < 10; ++i)
      {
          for (int j = 0; j < n_caracteristicas-1; ++j)
          {
              //Probabilidad de mutacion de cada gen de 0.001 es decir 1/1000
              int r = Randint(0,1000)
              if(r==1)
                mutacion(poblacion[i],j)
          }
      }

      //Introducimos al mejor individuo anterior por el peor

      if(!seleccionado){
            for (int i = 0; i < 10; ++i)
             {
             if(poblacion[i].tasa_clas < peor){
                 peor = poblacion[i].tasa_clas;
                 p_peor = i;
             }

            poblacion[p_peor] = mejor;
            }
        }
    }

    s = sol_copy(mejor);
    sol.tasa_red = calcularTasaRed();
    return s;
}


//Cada 10 generaciones se aplica la BL sobre un todos los cormosomas de la poblacion
*/

/************************ALGORITMOS MEMETICOS*************************/


s_caracteristicas MemeticoA (int inicio, int tam, int k, int n_interaciones){

    cout << "empieza el algoritmo Memetico" << endl;


    int n_cruces = 0.7 * (10 * 0.5);
    int n_padres = n_cruces * 2;
    int n_mutaciones = (10 * n_caracteristicas);
    int p_mejor=0;
    int p_peor=0;
    bool seleccionado = false;
    float peor = 100.0;
    int contador = 0;


    s_caracteristicas poblacion[10];
    s_caracteristicas padres[10];
    s_caracteristicas mejor;
    mejor.tasa_clas = 0.0;



    //Inicializamos la poblacion actual

    for (int i = 0; i < 10 ; ++i)
    {
        poblacion[i] = generarSolRandom();
        poblacion[i].tasa_clas = coste(inicio, tam, k, poblacion[i]);

    }

    int iteracion=10;

    while(iteracion<n_interaciones){



        //Seleccion del mejor individuo de la poblacion

     for (int i = 0; i < 10; ++i)
     {
        if(poblacion[i].tasa_clas>mejor.tasa_clas){
            mejor = sol_copy(poblacion[i]);
            p_mejor = i;
        }

     }

         //seleccion de los padres

      for (int i = 0; i < 10; ++i)
     {
         int r1 = Randint(0,9);
         int r2 = Randint(0,9);

          padres[i] = seleccionPadres(poblacion[r1],poblacion[r2]);

           if(r1==p_mejor || r2==p_mejor){
                seleccionado = true;
        }

     }



      //Cruce de padres

       for (int i = 0; i < n_padres; i+=2)
       {
           cruce(padres[i],padres[i+1],poblacion[i],poblacion[i+1]);
           poblacion[i].tasa_clas = coste(inicio,tam,k,poblacion[i]);
           poblacion[i+1].tasa_clas = coste(inicio,tam,k,poblacion[i+1]);
           iteracion+=2;
       }

       //Rellenamos la poblacion vacia con los padres

      for (int i = n_padres; i < 10; ++i){

          poblacion[i] = padres[i];
      }

        //Realizamos las mutaciones

      for (int i = 0; i < 10; ++i)
      {
          for (int j = 0; j < n_caracteristicas-1; ++j)
          {
              //Probabilidad de mutacion de cada gen de 0.001 es decir 1/1000
              int r = Randint(0,1000);
              if(r==1)
                mutacion(poblacion[i],j);
          }
      }

      //Introducimos al mejor individuo anterior por el peor

      if(!seleccionado){
            for (int i = 0; i < 10; ++i)
             {
             if(poblacion[i].tasa_clas < peor){
                 peor = poblacion[i].tasa_clas;
                 p_peor = i;
             }

            poblacion[p_peor] = mejor;
            }
        }


        if(contador%10==0){

            for (int i = 0; i < 10; ++i)
            {
                poblacion[i] = LocalSearch(inicio,tam ,k, poblacion[i],iteracion);
            }


        }
    }

    s_caracteristicas s;

    s = sol_copy(mejor);
    return s;
}


s_caracteristicas MemeticoB (int inicio, int tam, int k, int n_interaciones){


 cout << "empieza el algoritmo Memetico" << endl;


    int n_cruces = 0.7 * (10 * 0.5);
    int n_padres = n_cruces * 2;
    int n_mutaciones = (10 * n_caracteristicas);
    int p_mejor=0;
    int p_peor=0;
    bool seleccionado = false;
    float peor = 100.0;
    int contador = 0;


    s_caracteristicas poblacion[10];
    s_caracteristicas padres[10];
    s_caracteristicas mejor;
    mejor.tasa_clas = 0.0;



    //Inicializamos la poblacion actual

    for (int i = 0; i < 10 ; ++i)
    {
        poblacion[i] = generarSolRandom();
        poblacion[i].tasa_clas = coste(inicio, tam, k, poblacion[i]);

    }

    int iteracion=10;

    while(iteracion<n_interaciones){



        //Seleccion del mejor individuo de la poblacion

     for (int i = 0; i < 10; ++i)
     {
        if(poblacion[i].tasa_clas>mejor.tasa_clas){
            mejor = sol_copy(poblacion[i]);
            p_mejor = i;
        }

     }

         //seleccion de los padres

      for (int i = 0; i < 10; ++i)
     {
         int r1 = Randint(0,9);
         int r2 = Randint(0,9);

          padres[i] = seleccionPadres(poblacion[r1],poblacion[r2]);

           if(r1==p_mejor || r2==p_mejor){
                seleccionado = true;
        }

     }



      //Cruce de padres

       for (int i = 0; i < n_padres; i+=2)
       {
           cruce(padres[i],padres[i+1],poblacion[i],poblacion[i+1]);
           poblacion[i].tasa_clas = coste(inicio,tam,k,poblacion[i]);
           poblacion[i+1].tasa_clas = coste(inicio,tam,k,poblacion[i+1]);
           iteracion+=2;
       }

       //Rellenamos la poblacion vacia con los padres

      for (int i = n_padres; i < 10; ++i){

          poblacion[i] = padres[i];
      }

        //Realizamos las mutaciones

      for (int i = 0; i < 10; ++i)
      {
          for (int j = 0; j < n_caracteristicas-1; ++j)
          {
              //Probabilidad de mutacion de cada gen de 0.001 es decir 1/1000
              int r = Randint(0,1000);
              if(r==1)
                mutacion(poblacion[i],j);
          }
      }

      //Introducimos al mejor individuo anterior por el peor

      if(!seleccionado){
            for (int i = 0; i < 10; ++i)
             {
             if(poblacion[i].tasa_clas < peor){
                 peor = poblacion[i].tasa_clas;
                 p_peor = i;
             }

            poblacion[p_peor] = mejor;
            }
        }


        if(contador%10==0){

            for (int i = 0; i < 10; ++i)
            {
                //Cada 10 generaciones se aplica la BL sobre un subconjunto de cromosomas de la poblacion
                //seleccionando aleatoriamente con probabilidad pls=0.1 para cada cromosoma
                int r = Randint(0,10);

                if(r==1)
                    poblacion[i] = LocalSearch(inicio,tam ,k, poblacion[i],iteracion);

            }


        }
    }

    s_caracteristicas s;

    s = sol_copy(mejor);
    return s;
}


s_caracteristicas MemeticoC (int inicio, int tam, int k, int n_interaciones){


    cout << "empieza el algoritmo Memetico" << endl;


    int n_cruces = 0.7 * (10 * 0.5);
    int n_padres = n_cruces * 2;
    int n_mutaciones = (10 * n_caracteristicas);
    int p_mejor=0;
    int p_peor=0;
    bool seleccionado = false;
    float peor = 100.0;
    int contador = 0;


    s_caracteristicas poblacion[10];
    s_caracteristicas padres[10];
    s_caracteristicas mejor;
    mejor.tasa_clas = 0.0;



    //Inicializamos la poblacion actual

    for (int i = 0; i < 10 ; ++i)
    {
        poblacion[i] = generarSolRandom();
        poblacion[i].tasa_clas = coste(inicio, tam, k, poblacion[i]);

    }

    int iteracion=10;

    while(iteracion<n_interaciones){



        //Seleccion del mejor individuo de la poblacion

     for (int i = 0; i < 10; ++i)
     {
        if(poblacion[i].tasa_clas>mejor.tasa_clas){
            mejor = sol_copy(poblacion[i]);
            p_mejor = i;
        }

     }

         //seleccion de los padres

      for (int i = 0; i < 10; ++i)
     {
         int r1 = Randint(0,9);
         int r2 = Randint(0,9);

          padres[i] = seleccionPadres(poblacion[r1],poblacion[r2]);

           if(r1==p_mejor || r2==p_mejor){
                seleccionado = true;
        }

     }



      //Cruce de padres

       for (int i = 0; i < n_padres; i+=2)
       {
           cruce(padres[i],padres[i+1],poblacion[i],poblacion[i+1]);
           poblacion[i].tasa_clas = coste(inicio,tam,k,poblacion[i]);
           poblacion[i+1].tasa_clas = coste(inicio,tam,k,poblacion[i+1]);
           iteracion+=2;
       }

       //Rellenamos la poblacion vacia con los padres

      for (int i = n_padres; i < 10; ++i){

          poblacion[i] = padres[i];
      }

        //Realizamos las mutaciones

      for (int i = 0; i < 10; ++i)
      {
          for (int j = 0; j < n_caracteristicas-1; ++j)
          {
              //Probabilidad de mutacion de cada gen de 0.001 es decir 1/1000
              int r = Randint(0,1000);
              if(r==1)
                mutacion(poblacion[i],j);
          }
      }

      //Introducimos al mejor individuo anterior por el peor

      if(!seleccionado){
            for (int i = 0; i < 10; ++i)
             {
             if(poblacion[i].tasa_clas < peor){
                 peor = poblacion[i].tasa_clas;
                 p_peor = i;
             }

            poblacion[p_peor] = mejor;
            }
        }


        if(contador%10==0){


                //Cada 10 generaciones aplicar la BL sobre los 0.1·N mejores
                //cromosomas de la población actual (N es el tamaño de ésta).
                //Al ser 10 la población solo se aplica la BL sobre el mejor, dado que 0.1 * 10 = 1

                poblacion[p_mejor] = LocalSearch(inicio,tam ,k, poblacion[p_mejor],iteracion);

        }
    }

    s_caracteristicas s;

    s = sol_copy(mejor);
    return s;
}


int main(){

     Set_random(29061997);

    loadTable();
    shuffle();

    cout << "Tabla cargada: "<<path<<endl;
    int ini_entrena, ini_test, particiones[5];
    float tasa_red[10];
    float tasa_clas_test[10];
    float tasa_clas_entrena[10];
    double time[10];
    clock_t clk;
    int num_elementos = (int)(n_cadenas*0.5);
     for(int i = 0; i<5; i++)
        particiones[i]=Randint(0,n_cadenas-1);

    for(int i = 0; i<5; i++){
        cout<<(i*2)+1<<endl;
        clk = clock();
        ini_entrena =  particiones[i];
        ini_test = (ini_entrena+num_elementos)%n_cadenas;

         //   sol = MemeticoA(ini_entrena,num_elementos,3,15000);
          // sol = MemeticoB(ini_entrena,num_elementos,3,15000);
          sol = MemeticoC(ini_entrena,num_elementos,3,15000);
        //sol = greedySFS(ini_entrena, num_elementos ,3);

        tasa_red[i+i] = sol.tasa_red;
        tasa_clas_entrena[i+i] = sol.tasa_clas;
        tasa_clas_test[i+i] = coste(ini_test, num_elementos, 3,sol);
        time[i+i] = ((double)(clock()- clk)/CLOCKS_PER_SEC);
        cout<<(i*2)+2<<endl;
        clk = clock();
         // sol = MemeticoA(ini_test,num_elementos,3,15000);
        // sol = MemeticoB(ini_test,num_elementos,3,15000);
        sol = MemeticoC(ini_test,num_elementos,3,15000);
       // sol = greedySFS(ini_test, num_elementos ,3);

        tasa_red[i+i+1] = sol.tasa_red;
        tasa_clas_entrena[i+i+1] = sol.tasa_clas;
        tasa_clas_test[i+i+1] = coste(ini_entrena, num_elementos, 3,sol);
        time[i+i+1] = ((double)(clock()- clk)/CLOCKS_PER_SEC);
    }

     float tasa_clas_media=0.0;
    float tasa_red_media=0.0;
    for(int i=0; i<10; i++){
        cout<<"Particion "<<((int)((i*0.5)+1))<<"-"<<(i%2)+1<<":  ";
        cout<<"clas_test:"<<tasa_clas_test[i]<<"; clas_entrena:"<<tasa_clas_entrena[i]<<";  red:"<<tasa_red[i]<<";  T:"<<time[i]<<endl;
        tasa_clas_media += tasa_clas_test[i];
        tasa_red_media += tasa_red[i];
    }
    tasa_clas_media = tasa_clas_media / 10;
    tasa_red_media = tasa_red_media / 10;
    cout<<"tasa_red: "<<tasa_red_media<<endl;
    cout<<"tasa_clas: "<<tasa_clas_media<<endl;


    return 0;
}
