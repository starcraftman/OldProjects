package CORBAServer;


/**
* CORBAServer/Stock_s.java .
* Generated by the IDL-to-Java compiler (portable), version "3.2"
* from /home/starcraftman/programming/423DistributedProject/a2/423-a2/src/corba/base/interface.idl
* Sunday, November 4, 2012 6:35:54 o'clock AM EST
*/

public final class Stock_s implements org.omg.CORBA.portable.IDLEntity
{
  public String storeName = null;

  /* Store character, is unique ID. */
  public int num = (int)0;

  public Stock_s ()
  {
  } // ctor

  public Stock_s (String _storeName, int _num)
  {
    storeName = _storeName;
    num = _num;
  } // ctor

} // class Stock_s