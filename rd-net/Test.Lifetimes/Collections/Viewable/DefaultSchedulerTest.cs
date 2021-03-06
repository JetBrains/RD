using System;
using System.Threading;
using JetBrains.Collections.Viewable;
using JetBrains.Diagnostics;
using NUnit.Framework;

namespace Test.Lifetimes.Collections.Viewable
{
  public class DefaultSchedulerTest : LifetimesTestBase
  {
    [Test]
    public void SimpleTest()
    {
      var scheduler = DefaultScheduler.Instance;
      Assert.IsTrue(scheduler.OutOfOrderExecution);
      Assert.IsFalse(scheduler.IsActive);

      var thread = Thread.CurrentThread;

      var hasValue = new ManualResetEvent(false);
      scheduler.Queue(() =>
      {
        try
        {
          Assert.IsTrue(scheduler.IsActive);
          Assert.AreNotEqual(thread, Thread.CurrentThread);

          var executed = false;
          scheduler.InvokeOrQueue(() => executed = true);
          Assert.IsTrue(executed);
        }
        catch (Exception e)
        {
          TestLogger.ExceptionLogger.Error(e);
        }
        finally
        {
          hasValue.Set();
        }
      });

      Assert.IsTrue(hasValue.WaitOne(TimeSpan.FromSeconds(1)));
    }
  }
}