package QueensToSat;

import it.uniroma1.di.tmancini.utils.*;
import it.uniroma1.di.tmancini.teaching.ai.SATCodec.*;
import java.util.*;
import java.lang.Math;


public class queensToSat{
    public static void main(String args[]){
        int n=10;
        IntRange chessRange = new IntRange("chessRange", 1, n);
        RangeProduct coordRange = new RangeProduct("coordRange", chessRange, 2);

        SATEncoder encoder = new SATEncoder("NQueens", "NQueens_enc.cnf");
        encoder.defineFamilyOfVariables("Q", chessRange, chessRange);
        //encoder.enableDebugMode();

        Iterator<List<Integer>> coordIterator;


        // con tutto il blocco dico che ci deve essere esattamente una regina per riga
        for (int x : chessRange.values()) {
            for (int y : chessRange.values()) {
                //aggiungo Q[1,1] or ... or Q[1,n]
                encoder.addToClause("Q", x, y);
                
                
            }
            encoder.endClause();
            
            
            List<Integer> tempCoord;
            
            coordIterator = coordRange.iterator(RangeProduct.FILTER.ALLDIFF_ORDERED);
            while(coordIterator.hasNext()){

                tempCoord = coordIterator.next();
                System.out.println(tempCoord.get(0)+" "+ tempCoord.get(1));
                //vincoli -Q[1,1] or -Q[1,2] ecc
                encoder.addNegToClause("Q", x, tempCoord.get(0));
                encoder.addNegToClause("Q", x, tempCoord.get(1));
                encoder.endClause();
                } 
            
        }
        

        // qua devo dire esattamente una regina per colonna
        for (int y : chessRange.values()) {
            for (int x : chessRange.values()) {
                //aggiungo Q[1,1] or ... or Q[n,1]
                encoder.addToClause("Q", x, y);
                
            }
            encoder.endClause();
            
            List<Integer> tempCoord;
            coordIterator = coordRange.iterator(RangeProduct.FILTER.ALLDIFF_ORDERED);
            while(coordIterator.hasNext()){
                
                tempCoord = coordIterator.next();
                //System.out.println(tempCoord.toString());
                
                //vincoli -Q[1,1] or -Q[2,1] ecc
                encoder.addNegToClause("Q", tempCoord.get(0), y);
                encoder.addNegToClause("Q", tempCoord.get(1), y);
                encoder.endClause();
                } 
            
        }

        // ora devo inserire i vincoli per non avere regine in conflitto sulle diagonali

        //riprendo gli iteratori perché mi servono senza filtro, dato che ho bisogno anche delle coppie dove y=y1
        encoder.addComment("una regina per diagonale NO-SE");
        //clausole per la diagonale da in alto a sinistra a in basso a destra
        for(int x: chessRange.values()){
            for(int y: chessRange.values()){
                //prendo le coordinate della diagonale sotto la mia posizione
                int x2=x+1;
                int y2=y+1;
                while(chessRange.inBounds(x2) && chessRange.inBounds(y2)){
                    encoder.addNegToClause("Q", x, y);
                    encoder.addNegToClause("Q", x2, y2);
                    encoder.endClause();
                    x2++;
                    y2++;
                }
            }
        }

        encoder.addComment("una regina per diagonale NE-SO");
        //clausole per la diagonale da in alto a sinistra a in basso a destra
        for(int x: chessRange.values()){
            for(int y: chessRange.values()){
                //prendo le coordinate della diagonale sotto la mia posizione
                int x2=x+1;
                int y2=y-1;
                while(chessRange.inBounds(x2) && chessRange.inBounds(y2)){
                    encoder.addNegToClause("Q", x, y);
                    encoder.addNegToClause("Q", x2, y2);
                    encoder.endClause();
                    x2++;
                    y2--;
                }
            }
        }

        encoder.end();


    }
}