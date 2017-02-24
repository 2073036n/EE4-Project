import java.awt.image.BufferedImage;
import java.awt.image.DataBufferByte;
import java.io.IOException;
import javax.imageio.ImageIO;
import java.io.File;
import java.awt.Image;
import java.io.FileWriter;
import java.io.BufferedWriter;
import java.lang.Integer;
import java.util.Arrays;


/*
* Loops through all jpeg files in a directory defined by the variable dir and creates and saves a series of images,
* each 1x768 pixels, made up of the center pixels of the images in the directory. Also saves the integer values in a text file
* image_arrays.txt with each array on a seperate line.
*/
public class imageParse{

	public static void main(String args[]) throws IOException{

		File dir = new File("/home/timothy/workspace/TDP4/testDir4");
  		File[] directoryListing = dir.listFiles();
		if (directoryListing != null) {

			BufferedWriter output = null;
			String[] data = new String[directoryListing.length];
			Arrays.fill(data,"");
			File file = new File("image_arrays"+".txt");
			output = new BufferedWriter(new FileWriter(file));

			int col = 1;
			BufferedImage bi = new BufferedImage(1,768,BufferedImage.TYPE_BYTE_GRAY);

		    for (File child : directoryListing) {
		      	if( child.getName().substring(child.getName().lastIndexOf(".")+1).equals("jpg")){
		      		System.out.println(child.getName());

		      		BufferedImage im = null;

		      		im = ImageIO.read(child);
		      		final byte[] pixels = ((DataBufferByte) im.getRaster().getDataBuffer()).getData();
				    final int width = im.getWidth();
				    final int height = im.getHeight();
				    final boolean hasAlphaChannel = im.getAlphaRaster() != null;

				    if (hasAlphaChannel){
				    	boolean oneImage = true;
				    	final int pixelLength = 4;
				    	for (int pixel = 0, row=0; row < height && oneImage; row++){
				    		int sum = 0;
				    		sum += (((int) pixels[pixel + ((width/2)*pixelLength + row*(width*pixelLength))] & 0xff) << 24); // alpha
				            sum += ((int) pixels[pixel  + ((width/2)*pixelLength + row*(width*pixelLength)) + 1] & 0xff); // blue
				            sum += (((int) pixels[pixel + ((width/2)*pixelLength + row*(width*pixelLength)) + 2] & 0xff) << 8); // green
				            sum += (((int) pixels[pixel + ((width/2)*pixelLength + row*(width*pixelLength)) + 3] & 0xff) << 16); // red
				            
				            data[((child.getName().charAt(child.getName().lastIndexOf(".")-1)+10)%10)-1]+=sum+",";

				            bi.setRGB(0,col,sum);
				            col++;

				            if(col>=768){
				            	File ioutput = new File("line_"+(child.getName().charAt(child.getName().length()-1))+".jpg");
				            	ImageIO.write(bi,"jpg",ioutput);
				            	col=0;
				            	oneImage = false;
				            }
				    	}
				    }
				    else{
				    	final int pixelLength = 3;
				    	boolean oneImage = true;
				    	int t = (child.getName().charAt(child.getName().lastIndexOf(".")-1)-'0');
				    	if(t==0) t=10;
				        for (int pixel = 0, row=0; row < height && oneImage; row++) {
				        	int sum = 0;
				            sum += ((int) pixels[pixel + ((width/2)*pixelLength + row*(width*pixelLength))] & 0xff); // blue
				            sum += (((int) pixels[pixel + ((width/2)*pixelLength + row*(width*pixelLength)) + 1] & 0xff) << 8); // green
				            sum += (((int) pixels[pixel + ((width/2)*pixelLength + row*(width*pixelLength)) + 2] & 0xff) << 16); // red

				            data[t-1]+=String.format("%32s",sum).replace(" ","0");

				         	bi.setRGB(0,col,sum);
				            col++;

				            if(col>=768){
				            	File ioutput = new File("line_"+(t)+".jpg");
				            	ImageIO.write(bi,"jpg",ioutput);
				            	col=0;
				            	oneImage = false;
				            }
				        }
				    }	

		      	}
		    }
		    for(String s:data){
			    try {
					output.write(s);
				} catch ( IOException e ) {
					e.printStackTrace();
				}
			}
		    output.close();
		} 
		else {
		    System.out.println("Directory not found.");
		}
	}

}