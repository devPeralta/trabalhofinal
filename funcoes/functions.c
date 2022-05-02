#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "structs.h"
#define FILEMAXNOME 25
#define MAXDADOS 200
#define MAXRATINGS 24188078
#define MAXPLAYERS 18944
#define MAXTAGS 364950
#define MAXNOMEPLAYER 100
#define MAXNUMTAGS 10
#define MAXLENTAGS 5
#define TAMPLAYERS 2000

char *minuscula(char *str){

    size_t len = strlen(str);
    char *lower = calloc(len+1, sizeof(char));

    for (size_t i = 0; i < len; ++i) {
        lower[i] = tolower((unsigned char)str[i]);
    }

    return lower;

}

int hashCode(int key) {
   return key % HASHSIZE;
}

struct DataItem *search(int key) {
   //get the hash 
   
   int hashIndex = hashCode(key);  
    
   //move in array until an empty 
   while(hashArray[hashIndex] != NULL) {
	
      if(hashArray[hashIndex]->sofifaid == key)
      {
        return hashArray[hashIndex]; 
      }
      else{
      }
        
			
      //go to next cell
      ++hashIndex;
      
		
      //wrap around the table
      hashIndex %= HASHSIZE;
   }        
   return NULL;        
}

void insert(int key,float data) {
    int flag=0;
    //printf("key=%d -- data=%d", key, data);

   struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
   item->somaRating = data;  
   item->sofifaid = key;
   
   //get the hash 
   int hashIndex = hashCode(key);
   //item->qtdRating = hashArray[hashIndex]->qtdRating;

   //move in array until an empty or deleted cell
   while(hashArray[hashIndex] != NULL && hashArray[hashIndex]->sofifaid != -1 && flag==0) {

       if(hashArray[hashIndex]->sofifaid == key){
           item->somaRating = hashArray[hashIndex]->somaRating + data;
           item->qtdRating = hashArray[hashIndex]->qtdRating + 1;
           flag=1;
       }
       else{
           //go to next cell
            ++hashIndex;
            //wrap around the table
            hashIndex %= HASHSIZE;
       }

      
   }

   if(flag==0){
       item->qtdRating = 1;
   }
	
   hashArray[hashIndex] = item;
}

void saveRatings(FILE *file){

    char line[100];
    int cont, cont2=0;
    int num_userid, num_sofifaid;
    float num_rating;

    file = fopen("csvFiles/minirating.csv", "r");
    if (file == NULL)
    {
        perror("Nao foi possivel abrir o arquivo rating.csv.");
        exit(1);
    }

    while (fgets(line, sizeof(line), file))
    {
        cont=0;
        char s[2] = ",";
        char *token;
        token = strtok(line, s);

        while (token)
        {   
            if(cont2>0){
            if(cont2%1000==0){
            }
            cont++;
                if(cont==1){
                    num_userid = atoi(token);
                }
                else if(cont==2){
                    num_sofifaid = atoi(token);
                }
                else if(cont==3){
                    num_rating = atof(token);
                }
            }
            token = strtok(NULL, ",");
        }
        if(cont2>0){ 
            insert(num_sofifaid, num_rating);
            //insere_tabela(ht, num_sofifaid, num_userid, num_rating);
        }
        //printf("%d\n", cont2);
        cont2++;
    }

}

Lista *inicializaLista()
{
    return NULL;
}

Lista *insereLista(Lista *l, char dado[MAXDADOS]){
    Lista *novo = (Lista *)malloc(sizeof(Lista));
    strcpy(novo->dados, dado);
    novo->prox = l;
    return novo;
}

Lista *saveFile(FILE *file, char nomeArquivo[FILEMAXNOME], Lista *l)
{
    char teste[MAXDADOS];
    double cont = 0.0;

    char diretorio[FILEMAXNOME] = "";
    strcat(diretorio, "csvFiles/");
    strcat(diretorio, nomeArquivo);
    
    file = fopen(diretorio, "r");
    if (file == NULL)
    {
        perror("Nao foi possivel abrir o arquivo.");
        exit(1);
    }

    char line[200];

    while (fgets(line, sizeof(line), file))
    {
        char *token;

        token = strtok(line, "");

        while (token != NULL)
        {
            strcpy(teste,token);
            l = insereLista(l, teste);
            cont = cont + 1;
            //if(cont>12459999){ // max=12461413
            //printf("%.lf: %s",cont, l->dados);
            //}
            token = strtok(NULL, "");
        }
        
        
    }
    return l;
}

void le_player(char dado[MAXDADOS], char *word, int *id){

    int cont=0;
    int idsofifa;
    char tags[25] = "";
    char nome[MAXNOMEPLAYER], tag[MAXNUMTAGS][MAXLENTAGS];

    char s[2] = ",\"";
    char *token;
    token = strtok(dado, s);

    while (token != NULL)
    {
        if (cont == 0)
        {   
            idsofifa = atoi(token);
        }
        else if (cont == 1)
        {
            strcpy(nome, token);
        }
        else
        {
            strcat(tags, token);
        }

        token = strtok(NULL, s);

        cont++;
    }
    strcpy(word, nome);
    *id = idsofifa;

}

int trie_new (struct trie **trie)
{
    *trie = calloc(1, sizeof(struct trie));
    if (NULL == *trie) {
        // erro ao alocar memoria
        return -1;
    }
    return 0;
}

int trie_insert (struct trie *trie, char *word, unsigned word_len, int id, int *qtd, float *avg)
{
    int ret = 0, index;

    if (0 == word_len) {
        trie->end_of_word = true;
        trie->trie_sofifaid = id;
        trie->trie_qtdrating = qtd;
        trie->trie_avgrating = avg;
        return 0;
    }
    if(((int)word[0]) == 32){ // space
        index = 26;
    }
    else if(((int)word[0]) == 39){ // '
        index = 27;
    }
    else if(((int)word[0]) == 46){ // .
        index = 28;
    }
    else if(((int)word[0]) == 45){ // -
        index = 29;
    }
    else{
        index = word[0] - 'a';
    }

    if (ALPHABET_SIZE <= index) {
        return -1;
    }

    if (NULL == trie->children[index]) {
        ret = trie_new(&trie->children[index]);
        if (-1 == ret) {
            return -1;
        }
    }
    
    return trie_insert(trie->children[index], word + 1, word_len - 1, id, qtd, avg);
}

int trie_search(struct trie *trie, char *word, unsigned word_len,struct trie **result)
{
    int index;
    if (0 == word_len) {
        *result = trie;
        return 0;
    }

    if(((int)word[0]) == 32){ // space
        index = 26;
    }
    else if(((int)word[0]) == 39){ // '
        index = 27;
    }
    else if(((int)word[0]) == 46){ // .
        index = 28;
    }
    else if(((int)word[0]) == 45){ // -
        index = 29;
    }
    else{
        index = word[0] - 'a';
    }

    if (ALPHABET_SIZE <= index) {
        return -1;
    }

    if (NULL == trie->children[index]) {
        return -1;
    }

    return trie_search(trie->children[index], word + 1,word_len - 1, result);
}

void trie_print (struct trie *trie, char prefix[], unsigned prefix_len)
{      
    char aux[50];
    int len_aux;

    if (true == trie->end_of_word) {
        printf("%d ", trie->trie_sofifaid);

        itoa(trie->trie_sofifaid, aux, 10);
        len_aux = strlen(aux) + 2;
        //printf(" %d ", prefix_len);

        if(prefix_len+len_aux == 49){
            printf("%.*s", prefix_len, prefix);
        }
        else if(prefix_len+len_aux > 45){
            printf("%.*s\t", prefix_len, prefix);
        }
        else if(prefix_len+len_aux > 40){
            printf("%.*s\t", prefix_len, prefix);
        }
        else if(prefix_len+len_aux > 32){
            printf("%.*s\t\t", prefix_len, prefix);
        }
        else if(prefix_len+len_aux >= 25){
            printf("%.*s\t\t\t", prefix_len, prefix);
        }
        else if(prefix_len+len_aux > 16){
            printf("%.*s\t\t\t\t", prefix_len, prefix);
        }
        else{
            printf("%.*s\t\t\t\t\t", prefix_len, prefix);
        }
        
        printf("%.6f\t", *trie->trie_avgrating);
        printf("%d\n", *trie->trie_qtdrating);
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {

        if (NULL == trie->children[i]){
            continue;
        }

        if(i+'a' == '{'){
            prefix[prefix_len] = ' ';
        }
        else if(i+'a' == '|'){
            prefix[prefix_len] = '\'';
        }
        else if(i+'a' == '}'){
            prefix[prefix_len] = '.';
        }
        else if(i+'a' == '~'){
            prefix[prefix_len] = '-';
        }
        else{
            prefix[prefix_len] = i + 'a';
        }
        trie_print(trie->children[i], prefix, prefix_len + 1);
    }
}