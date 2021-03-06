// Worker base class.
class VxWorker {
    constructor() {
        this.name = '';
        this.vnotex = null;

        if (!window.vxWorkerId) {
            window.vxWorkerId = 1;
        }
        this.id = window.vxWorkerId++;
    }

    // Called when registering this worker.
    register(p_vnotex) {
        this.vnotex = p_vnotex;

        this.registerInternal();
    }

    registerInternal() {
        console.warning('RegisterInternal of VxWorker subclass is not implemented', this.name);
    }

    finishWork() {
        console.log('worker finished', this.name);
        this.vnotex.finishWorker(this.name);
    }
}
