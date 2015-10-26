import processing.pdf.*;
import de.bezier.data.*;
import processing.serial.*;

/* Serial Parameter */
int MJMACPort =11;
Serial myPort;                       // The serial port

/* Extract data from files */
XlsReader reader;
//String[][]RefugeeData = null;
int  numData= 1180;
int  numIndex=  3;
int TotalnumOfRefugee;
int numMAXofActualData;
String[][]RefugeeData = new String[numData][numIndex];
String[][]ActualData = new String[numData][2];
 
int interval = 2000; // 2s
int time;
PImage worldMap;
void setup() {
  
  size(1250,630);
  reader = new XlsReader( this, "countryNames.xls");
  reader.nextRow();
  worldMap = loadImage("worldMap.png");
  image(worldMap, 0, 0, width, height);
  noStroke();
  frameRate(30);
  smooth();
  String portName = Serial.list()[MJMACPort];
  myPort = new Serial(this, portName, 9600);
  time = millis();
  getRefugeesData();
  getSortData();
}
void draw(){

    //reader = new XlsReader( this, "countryNames.xls");
    textAlign(CENTER);
    textSize(28);
    fill(235,235,235);
    text (TotalnumOfRefugee, 1100,557);
    noStroke();
    fill(0);
    textSize(10);
    
    reader.hasMoreRows();
   // while (reader.hasMoreRows()){
        reader.nextRow(); 
        for (int i=0; i < numMAXofActualData; i++)  //Count the number of data
        {
          try{  
              reader.firstCell();
              reader.nextCell();
              float y = map(reader.getFloat(), 90, -90f, 0f, height);
              reader.nextCell();
              float x = map(reader.getFloat(), -180f, 180f, 0f, width); 
              reader.nextCell();
              //String 
              String placeName = reader.getString();
              //reader.nextCell();//bypass cells
              //reader.nextCell();
              //reader.nextCell();
              //float dim = map(reader.getInt(), 0, 1330044000, 3, 15);
              textSize(11);

              if (placeName.equals(ActualData[i][0]))
              {
                  println(i + ":placeName " + placeName + " Refegee " + ActualData[i][0]);
                  fill(290, 290, 290);
                  textLeading(14);
                  text(placeName + "\n(" + ActualData[i][1] +")", x, y);  //the number of refugees
                   //myPort.write(placeName + ":" + ActualData[i][1]);
                  delay(600);
                  fill(255, 255, 255);
                  float f = new Float(ActualData[i][1]);
                  if (f <= 10)
                  {
                    fill(130,130,130,80);
                    ellipse(x, y, 40,40);
                  }
                  else if ((10<= f) && (f < 100))
                  {
                    fill(255,102,13,80);
                    ellipse(x, y, 45, 45);
                  }
                  else if ((100<= f) && (f < 500))
                  {
                    fill(85,130,0,60);
                    ellipse(x, y,48,48);
                  }
                  else if ((500<= f) && (f <3000))
                  {
                    fill(255,8,57,60);
                    ellipse(x, y, 60, 60);
                  }
                  else if ((3000<= f) && (f < 10000))
                  {
                    fill(232,234,130,70);
                    ellipse(x, y, 70,70);
                  }
                  else
                  {
                    fill(77,1,0,70);
                    ellipse(x, y,80,80);
                  }
                  //ellipse(x, y, 50, 50);
                  //println(placeName);
                  myPort.write(placeName + ":" + ActualData[i][1]);
              }
              else{
                fill(200,200,200, 0);
                text(placeName, x, y);
              }

          }catch (Exception e) {
          println(e);
        }
       }
  // }
}

void getRefugeesData(){
  XML xml = loadXML("UNdata_Export_20151023_124411157.xml");//get xml data
  XML[] record = xml.getChildren("data");
  
  for (int i = 0; i < record.length; i++) {
    //println(record[i].getContent()); 
    XML[] field = record[i].getChildren("record");
    for (int j = 0; j < field.length; j++) {
     // println(j + "," + field[j].getContent()); 
       //println("===================" + j +"===================");
       XML[] data = field[j].getChildren("field");
      // RefugeeData = new String[j][];
       
       for (int k = 0; k < data.length; k++)
       {
         /*
         if (k == 0)
         {
           println("data" + data.length);
           RefugeeData[j] = new String[data.length];
         }*/   
         //println(j + "," + k + "," + data[k].getContent()); 
         
          if (k == 0)//Destination
          {
            RefugeeData[j][0] = data[k].getContent();
          }
          else if (k == 2)//year
          {
            RefugeeData[j][1] = data[k].getContent();
          }
          else if (k == 5)//Total refugees
          {
            RefugeeData[j][2] = data[k].getContent();
          }
       }
    }
   // println("field" + field.length);
    
  } 
}
void getSortData(){
  
 println("Sort Auctual data");
 int MAXYear = 2013; 
 int MINYear = 1988;
 int index=0;
 
 boolean flagAdd =  true;
 
 for (index=0; index < numData; index++) //Put 2013's data into ActualData array.
 {
   int year = parseInt(RefugeeData[index][1]);
   if (year != 2013)
   {
     break;
   }
   //println("Country:" + RefugeeData[index][0]);
   ActualData[index][0] = RefugeeData[index][0] ;//Country
   ActualData[index][1] = RefugeeData[index][2] ;//Population of Refugee
   //println("Destination:" + ActualData[index][0] + "Population" + ActualData[index][1]);
 }
 numMAXofActualData = index-1;
 //println ("The number of refugees in 2013:" + numMAXofActualData);
 //println("Destination:" + ActualData[numMAXofActualData][0] + "Population" + ActualData[numMAXofActualData][1]);
 
 for (int j=index; j < numData; j++) //The loop of RefugeData
 {
   for(int k=0; k < numMAXofActualData; k++) //The loop of ActualData: to check whether the country is already existed in the actualData array.
   {
     //println("Actual:"+ ActualData[k][0]+"new:" + RefugeeData[j][0]);

     if (ActualData[k][0].equals(RefugeeData[j][0]))
     {
        println("Actual:"+ ActualData[k][0]+"new:" + RefugeeData[j][0]);
        int num1 =parseInt(ActualData[k][1]);
        int num2 =parseInt(RefugeeData[j][2]);
        ActualData[k][1] = Integer.toString(num1 + num2);
        flagAdd =false;  //change flag to flase because new data already added with existed data.
     }
   }
   if(flagAdd==true)  //Add new country 
    {
      numMAXofActualData++;
      ActualData[numMAXofActualData][0] = RefugeeData[j][0];
      ActualData[numMAXofActualData][1] = RefugeeData[j][2];
    }
    else
    {
      flagAdd =true;
    }
 }
 
 println("Actual:"+ ActualData[numMAXofActualData][0] + "Population:"+ActualData[numMAXofActualData][1]);
 
 for (int num=0; num < (numMAXofActualData); num++)
 {
   int temp = parseInt(ActualData[num][1]);
   TotalnumOfRefugee = TotalnumOfRefugee + temp;
 }
 println("The actual the number of countries is " + numMAXofActualData + "\n Total the number of refugees:"+ TotalnumOfRefugee);
 
}