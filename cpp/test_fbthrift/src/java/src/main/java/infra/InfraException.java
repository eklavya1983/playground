package infra;

import infra.gen.Status;

/**
 * Created by nbayyana on 9/20/16.
 */
public class InfraException extends Exception
{
    public InfraException(Status status)
    {
        this.status = status;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    private Status status;
}
